#include "../pch.hpp"
#include "CharacteristicManagement.hpp"

namespace JFMService
{

	CharacteristicLoader::CharacteristicLoader()
	{
		loaders[".dat"] = std::bind(&CharacteristicLoader::loadDatContent, this, std::placeholders::_1);
	}

	CharacteristicData CharacteristicLoader::load(const std::filesystem::path& path)
	{
		// Check if the file extension is supported
		if (checkExtentionCompatibility(path))
			return CharacteristicData();

		// Read the file content
		std::ifstream file(path);
		std::string content;

		if (file)
		{
			std::stringstream buffer;
			buffer << file.rdbuf();
			content = buffer.str();
			file.close();
		}

		// Use the loader function to parse the content
		auto data = loaders.at(path.extension().string())(content);

		// Extract additional metadata
		std::string name = readName(path.filename().string());
		double temperature = readTemperature(path.filename().string());

		return CharacteristicData(data, temperature, name);
	}

	// Method to load .dat content and parse it into vectors
	std::vector<std::vector<double>> CharacteristicLoader::loadDatContent(std::string& content)
	{
		std::vector<std::string> lines = utils::spliting(content, "\n");
		int size = lines.size();
		std::vector<double> voltages(size), currents(size), densityCurrents(size);

		auto parseColumn = [&](int index, const std::string& line)
			{
				auto splittedLine = utils::spliting(line, "\t");
				if (splittedLine.size() == 3)
					return std::stod(splittedLine[index]);
			
			};
		std::vector<std::vector<double>> items = { voltages, currents, densityCurrents };

		for (const auto& [i, dest] : std::views::enumerate(items) )
			std::ranges::transform(lines, dest.begin(), [&](const std::string& line) {
				return parseColumn(i, line);  
				});
		return items;
	}

	double CharacteristicLoader::readTemperature(const std::string& name)
	{
		auto it = name.find("T");
		if (it != std::string::npos)
			return std::stod(name.substr(it + 1, 3));

		return -1;
	}

	std::string CharacteristicLoader::readName(const std::filesystem::path& path)
	{
		return path.stem().string();
	}

	bool CharacteristicLoader::checkExtentionCompatibility(const std::filesystem::path& path) const 
	{
		return loaders.find(path.extension().string()) == loaders.end();
	}

	CharacteristicData::CharacteristicData(std::vector<std::vector<double>>& data, double temperature, const std::string& name)
		:Temperature(temperature), Name(name)
	{
		Data.resize(data.size());
		for (const auto& [ dest, src] : std::views::zip(Data,data))
			dest =  std::move(src);
	}
}
#include "Loader.hpp"
#include "IDataManager.hpp"
#include "JFMIFitting.hpp"
#include "JFMFitter.hpp" //todo manage includes

#include <yaml-cpp/yaml.h>

namespace JFMService
{

	//! CharacteristicLoader
	CharacteristicLoader::CharacteristicLoader()
	{
		loaders[".dat"] = std::bind(&CharacteristicLoader::loadDatContent, this, std::placeholders::_1);
	};
	LoaderOutput CharacteristicLoader::Load(const std::filesystem::path &path)
	{
		if (!CheckExtentionCompatibility(path))
			return LoaderOutput();

		std::string content;

		readFileContent(path, content);

		LoaderOutput output;
		parseContent(path, content, output);
		return output;
	};

	void CharacteristicLoader::readFileContent(const std::filesystem::path &path, std::string &content)
	{
		std::ifstream file(path);
		if (file)
		{
			std::stringstream buffer;
			buffer << file.rdbuf();
			content = buffer.str();
			file.close();
		}
	};
	void CharacteristicLoader::parseContent(const std::filesystem::path &path, std::string &content, LoaderOutput &output)
	{
		auto ex = path.extension().string();
		auto data = loaders.at(ex)(content);
		double temperature = readTemperature(path.filename().string());
		std::string name = readName(path);
		output.data = std::move(std::make_unique<DataManagementService::CharacteristicData>(data, temperature, name));
		output.success = true;
	}

	std::vector<std::vector<double>> CharacteristicLoader::loadDatContent(std::string &content)
	{
		std::vector<std::string> lines = utils::spliting(content, "\n");
		int size = lines.size();
		std::vector<double> voltages{}, currents{}, densityCurrents{};
		std::vector<std::vector<double>> items = {voltages, currents, densityCurrents};

		for (const auto &line : lines)
		{
			auto splittedLine = utils::spliting(line, "\t");
			if (splittedLine.size() == 3)
			{
				items[0].push_back(std::stod(splittedLine[0]));
				items[1].push_back(std::stod(splittedLine[1]));
				items[2].push_back(std::stod(splittedLine[2]));
			}
		}
		return items;
	}

	bool CharacteristicLoader::CheckExtentionCompatibility(const std::filesystem::path &path)
	{
		bool found = loaders.find(path.extension().string()) != loaders.end();
		return found;
	};

	double CharacteristicLoader::readTemperature(const std::string &name)
	{
		auto it = name.find("T");
		if (it != std::string::npos)
			return std::stod(name.substr(it + 1, 3));

		return -1;
	};
	std::string CharacteristicLoader::readName(const std::filesystem::path &path) { return path.stem().string(); };

	//! MCLoader
	LoaderOutput MonteCarloResultLoader::Load(const std::filesystem::path &path)
	{

		std::ifstream stream(path.c_str());
		std::stringstream strStream;
		strStream << stream.rdbuf();

		using namespace FittingService;
		LoaderOutput output;
		MCOutput loadedData;
		MCInput inputToFitting;

		YAML::Node data = YAML::Load(strStream.str());
		auto config = data["config"];
		loadConfig(config, inputToFitting);
		std::vector<ParameterName> order = config["order"].as<std::vector<ParameterName>>();
		loadedData.inputData = inputToFitting;
		auto d = data["data"];
		loadedData.mcResult = loadFittingResults(d, order, inputToFitting.iterations);

		output.mcData = std::make_unique<FittingService::MCOutput>(loadedData);
		return output;
	}
	bool MonteCarloResultLoader::CheckExtentionCompatibility(const std::filesystem::path &path)
	{
		return path.extension() == ".yaml";
	}
	void MonteCarloResultLoader::loadConfig(const YAML::Node &config, MCInput &destination)
	{

		auto modelNameToID = [](const std::string &name)
		{
			if (name == "FourParameterModel")
				return Model4P;
			if (name == "SixParameterModel")
				return Model6P;
		};

		std::string model = config["model"].as<std::string>();
		std::string relativePath = config["relativePath"].as<std::string>();
		std::string name = config["CharacteristicName"].as<std::string>();

		std::vector<double> idealParameters = config["idealParameters"].as<std::vector<double>>();
		std::vector<std::string> order = config["order"].as<std::vector<std::string>>();

		double Temperature = config["Temperature"].as<double>();
		int iterationNumber = (int)config["numberOfIterations"].as<double>();
		std::vector<std::string> fixingConfiguration = config["fixingConfiguration"].as<std::vector<std::string>>();
		std::vector<double> fixingValues = config["fixedValues"].as<std::vector<double>>();
		double noise = config["noise"].as<double>();

		std::pair<double, double> characteristicIndexes{config["CharacteristicLowRange"].as<int>(), config["CharacteristicUpRange"].as<int>()};

		destination.startingData.initialData.modelID = modelNameToID(model);
		destination.relPath = relativePath;
		destination.startingData.name = name;
		transferParameters(destination.trueParameters, order, idealParameters);
		destination.startingData.initialData.additionalParameters[Fitters::AdditionalParametersID::Temperature] = Temperature;
		destination.iterations = iterationNumber;
		destination.characteristicBounds = characteristicIndexes;
		// FixingConfiguration fixingConfig = (FixingConfiguration)0;
		ParameterMap additional;

		for (const auto &[name, value] : std::views::zip(fixingConfiguration, fixingValues))
			additional[parameterNameToID(name)] = value;
		destination.startingData.fixConfig = additional;
		destination.noise = noise;
	}
	std::vector<MCResult> MonteCarloResultLoader::loadFittingResults(const YAML::Node &node, std::vector<ParameterName> &order, size_t size)
	{
		using namespace FittingService;
		auto transferArrayToMap = [&](std::vector<double> params)
		{
			ParameterMap parameters;
			transferParameters(parameters, {"A", "I0", "Rs", "Rsh", "alpha", "Rsh2"}, params);
			return parameters;
		};
		std::vector<MCResult> result{size};
		MCResult singleResult;
		for (const auto &[item, dst] : std::views::zip(node, result))
		{
			auto i = item["result"]["parameters"].as<std::vector<double>>();
			singleResult.foundParameters = transferArrayToMap(i);
			singleResult.error = item["result"]["error"].as<double>();
			dst = singleResult;
		}
		return result;
	}
	void MonteCarloResultLoader::transferParameters(ParameterMap &destination, const std::vector<ParameterName> names, const std::vector<double> values)
	{
		for (const auto &[name, value] : std::views::zip(names, values))
			destination[parameterNameToID(name)] = value;
	};

}
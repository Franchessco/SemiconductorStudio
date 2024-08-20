#pragma once
#include "pch.hpp"
#include "utils.hpp"

namespace JFMService
{

	class CharacteristicData;

	class CharacteristicLoader
	{

	public:
		using ReturningType = CharacteristicData;
		CharacteristicLoader();
		~CharacteristicLoader() = default;

		CharacteristicData load(const std::filesystem::path &path);
		bool checkExtentionCompatibility(const std::filesystem::path &path) const;
		std::string getLoaderName() { return name; };

	private:
		std::unordered_map<std::string, std::function<std::vector<std::vector<double>>(std::string &)>> loaders;
		std::string name{"Characteristic Loader"};
		std::vector<std::vector<double>> loadDatContent(std::string &content);
		double readTemperature(const std::string &name);
		std::string readName(const std::filesystem::path &path);
	};

	class CharacteristicSerializer
	{
	};

}
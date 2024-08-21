#pragma once
#include "pch.hpp"
#include "utils.hpp"
#include "DataManager.hpp"
namespace JFMService
{

    class Loader
    {
    public:
        virtual bool CheckExtentionCompatibility(const std::filesystem::path &path) = 0;
        virtual DataManagementService::LoaderOutput Load(const std::filesystem::path &path) = 0;
    };

    class CharacteristicLoader : public Loader
    {
        using Output = DataManagementService::LoaderOutput;
        using loaderFunction = std::function<std::vector<std::vector<double>>(std::string &content)>;

    public:
        CharacteristicLoader();
        virtual DataManagementService::LoaderOutput Load(const std::filesystem::path &path) override;
        virtual bool CheckExtentionCompatibility(const std::filesystem::path &path) override;

    private:
        std::unordered_map<std::string, loaderFunction> loaders;

    private:
        std::vector<std::vector<double>> loadDatContent(std::string &content)
        {
            std::vector<std::string> lines = utils::spliting(content, "\n");
            int size = lines.size();
            std::vector<double> voltages(size), currents(size), densityCurrents(size);

            auto parseColumn = [&](int index, const std::string &line)
            {
                auto splittedLine = utils::spliting(line, "\t");
                if (splittedLine.size() == 3)
                    return std::stod(splittedLine[index]);
            };
            std::vector<std::vector<double>> items = {voltages, currents, densityCurrents};

            for (const auto &[i, dest] : std::views::enumerate(items))
                std::ranges::transform(lines, dest.begin(), [&](const std::string &line)
                                       { return parseColumn(i, line); });
            return items;
        };
        double readTemperature(const std::string &name);
        std::string readName(const std::filesystem::path &path);
        void parseContent(const std::filesystem::path &path, std::string &content, JFMService::CharacteristicLoader::Output &output);

        void readFileContent(const std::filesystem::path &path, std::string &content);
    };
}
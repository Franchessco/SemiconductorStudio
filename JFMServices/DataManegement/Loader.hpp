#pragma once
#include "pch.hpp"
#include "utils.hpp"
#include "IDataManager.hpp"
namespace JFMService
{
    using namespace DataManagementService;
    class Loader
    {
    public:
        virtual bool CheckExtentionCompatibility(const std::filesystem::path &path) = 0;
        virtual LoaderOutput Load(const std::filesystem::path &path) = 0;
    };

    class CharacteristicLoader : public Loader
    {
        using loaderFunction = std::function<std::vector<std::vector<double>>(std::string &content)>;

    public:
        CharacteristicLoader();
        virtual LoaderOutput Load(const std::filesystem::path &path) override;
        virtual bool CheckExtentionCompatibility(const std::filesystem::path &path) override;

    private:
        std::unordered_map<std::string, loaderFunction> loaders;

    private:
        std::vector<std::vector<double>> loadDatContent(std::string &content);
        void readFileContent(const std::filesystem::path &path, std::string &content);
        void parseContent(const std::filesystem::path &path, std::string &content, LoaderOutput &output);
        double readTemperature(const std::string &name);
        std::string readName(const std::filesystem::path &path);
    };

    class MonteCarloResultLoader : public Loader
    {
    public:
        MonteCarloResultLoader() = default;
        virtual LoaderOutput Load(const std::filesystem::path &path) override;
        virtual bool CheckExtentionCompatibility(const std::filesystem::path &path) override;

    private:
    };
}
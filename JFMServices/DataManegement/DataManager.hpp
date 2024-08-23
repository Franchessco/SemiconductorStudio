#pragma once
#include "pch.hpp"
#include "utils.hpp"
#include "IDataManager.hpp"
#include "Loader.hpp"
namespace JFMService
{
    enum class LoadingTypes
    {
        Characteristic = 0,
        MonteCarloResult
    };
    class CharacteristicLoader;
    class DataManager : public DataManagementService::IDataManager
    {
        using LoaderOutput = DataManagementService::LoaderOutput;

    public:
        virtual void load(const path &path, const Callback &callback) override;
        virtual void load(const std::vector<path> &paths, const VectorCallback &callbacks) override;

        virtual void save(const path &path, const LoaderOutput &input, const Callback &callback) override;
        virtual void save(const std::vector<path> &paths, const std::vector<LoaderOutput> &input, const VectorCallback &callbacks) override;

        virtual void MoveInto(path &currentPath, const std::string &destination) override;
        virtual void MoveBack(path &currentPath) override;

        virtual std::vector<std::string> GetDirectories(const path &path) override;
        virtual std::vector<std::string> GetFiles(const path &path) override;

    private:
        std::unordered_map<LoadingTypes, std::shared_ptr<Loader>> loaders;
        bool useThreadsParallel{ false };
        std::mutex mutex;

    private:
        LoadingTypes findLoader(const std::filesystem::path &path);
    };

}

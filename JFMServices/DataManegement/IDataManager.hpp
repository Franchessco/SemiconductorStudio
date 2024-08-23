#pragma once
#include "pch.hpp"
#include "../Fitting/JFMIFitting.hpp"
namespace JFMService::DataManagementService
{
    struct CharacteristicData
    {
    public:
        enum DataNames : unsigned int // todo move this into app site
        {
            Voltage = 0,
            Current,
            DensityCurrent
        };

        CharacteristicData() = default;
        CharacteristicData(std::vector<std::vector<double>> &data, double temperature, const std::string &name);

        std::vector<std::vector<double>> Data;
        double Temperature{-1};
        std::string Name;

        std::vector<double> &operator[](unsigned int name) { return Data.at(name); }
    };
    struct LoaderOutput
    {

        std::unique_ptr<CharacteristicData> data{nullptr};
        std::unique_ptr<FittingService::MCOutput> mcData{nullptr};
        bool success{};
    };

    class IDataManager
    {
    public:
        using Callback = std::function<void(LoaderOutput)>;
        using VectorCallback = std::function<void(std::vector<LoaderOutput>)>; // todo change this type to be more suitable
        using path = std::filesystem::path;

        virtual void load(const path &path, const Callback &callback) = 0;
        virtual void load(const std::vector<path> &paths, const VectorCallback &callbacks) = 0;

        virtual void save(const path &path, const LoaderOutput &input, const Callback &callback) = 0;
        virtual void save(const std::vector<path> &paths, const std::vector<LoaderOutput> &input, const VectorCallback &callbacks) = 0;

        virtual void MoveInto(path &currentPath, const std::string &destination) = 0;
        virtual void MoveBack(path &currentPath) = 0;

        virtual std::vector<std::string> GetDirectories(const path &path) = 0;
        virtual std::vector<std::string> GetFiles(const path &path) = 0;
    };
}
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
        virtual void load(const std::vector<path> &paths, const VectorCallback &callback) override;

        virtual void save(const path &path, const LoaderOutput &input, const Callback &callback) override;
        virtual void save(const std::vector<path> &paths, const std::vector<LoaderOutput> &input, const VectorCallback &callback) override;

        virtual void MoveInto(path &currentPath, const std::string &destination) override;
        virtual void MoveBack(path &currentPath) override;

        virtual std::vector<std::string> GetDirectories(const path &path) override;
        virtual std::vector<std::string> GetFiles(const path &path) override;

    private:
        std::unordered_map<LoadingTypes, std::shared_ptr<Loader>> loaders;

    private:
        LoadingTypes findLoader(const std::filesystem::path &path);
    };
    /*

    template <class... Types>
    class DataManager : public IDataLoader
    {
    public:
        DataManager() = default;
        DataManager(Types... args)
            : loaders{std::make_pair(args.getLoaderName(), std::variant<Types...>(args))...} {}

        ~DataManager() = default;

        void load(const std::filesystem::path& path, std::function<void()> callback = nullptr)
        {
            //using ReturnedType = std::variant<Types::ReturiningType...>;
            for (auto& item : loaders)
            {
                auto& loader = item.second;
                using LoaderType = std::decay_t<decltype(loader)>;
                using ConstructedType = LoaderType::ReturningType;
                ConstructedType constructed = loadData(loader,path);
                std::visit([&](auto& loader) {

                    if (loader.checkExtensionCompatibility(path))
                    {
                        auto constructed = loadData<LoaderType>(loader, path);
                    }
                    }, item.second);
            }
            if (callback)
                callback();

        }


        void load(const std::filesystem::path &path, std::function<void(LoaderOutput)> callback) override
        {

            for (auto &[key, loaderVariant] : loaders)

                auto visitor = [&](auto &&loader)
                {
                    // using LoaderType = std::decay_t<decltype(loader)>;

                    // std::cout << "Processing loader of type: " << typeid(LoaderType).name() << std::endl;

                    // if constexpr (Loader<LoaderType, typename LoaderType::ReturningType>) {
                    std::cout << "Loader satisfies the Loader concept." << std::endl;
                    std::cout << "Checking extension compatibility..." << std::endl;

                    if (loader.checkExtensionCompatibility(path))
                    {
                        std::cout << "Extension is compatible. Loading data..." << std::endl;
                        auto data = loadData(loader, path);
                        std::cout << "Data loaded successfully." << std::endl;
                    }
                    //}

                    std::visit(visitor, loaderVariant);
                } if (callback) callback();
        };

        void load(const std::vector<std::filesystem::path> &paths, std::function<void(std::vector<LoaderOutput>)> callback) override {};

    private:
        std::map<std::string, std::variant<Types...>> loaders;
        template <typename T>
        T &get_by_type() { return std::get<T>(loaders); };

        // template <class ConstructingType, Loader<ConstructingType> DataManager>
        template <class ConstructingType, class DataManager>
        ConstructingType loadData(DataManager loader, const std::filesystem::path &path)
        {
            return loader.load(path);
        }
    };
    */
}

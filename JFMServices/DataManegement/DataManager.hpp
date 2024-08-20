#pragma once
#include "pch.hpp"
#include "utils.hpp"
#include "concepts.hpp"
#include "IDataLoader.hpp"

namespace JFMService
{

    class CharacteristicLoader;
    template <class... Types>
    class DataLoader : public IDataLoader
    {
    public:
        DataLoader() = default;
        DataLoader(Types... args)
            : loaders{std::make_pair(args.getLoaderName(), std::variant<Types...>(args))...} {}

        ~DataLoader() = default;
        /*
        void load(const std::filesystem::path& path, std::function<void()> callback = nullptr)
        {
            //using ReturnedType = std::variant<Types::ReturiningType...>;
            for (auto& item : loaders)
            {
                auto& loader = item.second;
                using LoaderType = std::decay_t<decltype(loader)>;
                using ConstructedType = LoaderType::ReturningType;
                ConstructedType constructed = loadData(loader,path);
                /*std::visit([&](auto& loader) {

                    if (loader.checkExtensionCompatibility(path))
                    {
                        auto constructed = loadData<LoaderType>(loader, path);
                    }
                    }, item.second);
            }
            if (callback)
                callback();

        }
        */

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

        // template <class ConstructingType, Loader<ConstructingType> DataLoader>
        template <class ConstructingType, class DataLoader>
        ConstructingType loadData(DataLoader loader, const std::filesystem::path &path)
        {
            return loader.load(path);
        }
    };
}

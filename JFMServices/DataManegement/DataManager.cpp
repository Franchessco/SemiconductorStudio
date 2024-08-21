#include "DataManager.hpp"

namespace JFMService
{

    void DataManager::load(const path &path, const Callback &callback)
    {
        std::shared_ptr<Loader> loader = loaders.at(findLoader(path));
        LoaderOutput output = loader->Load(path);
        if (callback)
            callback(std::move(output));
    }

    void DataManager::load(const std::vector<path> &paths, const VectorCallback &callbacks)
    {
        for (const auto &[path, callback] : std::views::zip(paths, callbacks))
            load(path, callback);
    }

    void DataManager::save(const path &path, const LoaderOutput &input, const Callback &callback)
    {
    }

    void DataManager::save(const std::vector<path> &paths, const std::vector<LoaderOutput> &input, const VectorCallback &callbacks)
    {
    }

    void DataManager::MoveInto(path &currentPath, const std::string &destination) { currentPath /= destination; }

    void DataManager::MoveBack(path &currentPath) { currentPath = currentPath.parent_path(); }

    std::vector<std::string> DataManager::GetDirectories(const path &path)
    {
        std::vector<std::string> paths;
        for (const auto &item : std::filesystem::directory_iterator(path))
            if (item.is_directory())
                paths.push_back(item.path().filename().string());
        return paths;
    }

    std::vector<std::string> DataManager::GetFiles(const path &path)
    {
        std::vector<std::string> paths;
        for (const auto &item : std::filesystem::directory_iterator(path))
            if (!item.is_directory())
                paths.push_back(item.path().filename().string());
        return paths;
    }
    LoadingTypes DataManager::findLoader(const std::filesystem::path &path)
    {
        // todo improve this logic later, add method in loader which will check possibility of loading this type of file
        for (const auto &[loadingType, loader] : loaders)
        {
            if (!loader->CheckExtentionCompatibility(path))
                continue;
            else
                return loadingType;
        }
    }
}
#include "Loader.hpp"
namespace JFMService
{
    CharacteristicLoader::CharacteristicLoader()
    {
        loaders["dat"] = std::bind(&CharacteristicLoader::loadDatContent, this, std::placeholders::_1);
    };

    DataManagementService::LoaderOutput CharacteristicLoader::Load(const std::filesystem::path &path)
    {
        if (!CheckExtentionCompatibility(path))
            return DataManagementService::LoaderOutput();

        std::string content;

        readFileContent(path, content);

        Output output;
        parseContent(path, content, output);
        return output;
    };
    bool CharacteristicLoader::CheckExtentionCompatibility(const std::filesystem::path &path)
    {
        return loaders.find(path.extension().string()) == loaders.end();
    };

    double CharacteristicLoader::readTemperature(const std::string &name)
    {
        auto it = name.find("T");
        if (it != std::string::npos)
            return std::stod(name.substr(it + 1, 3));

        return -1;
    };
    std::string CharacteristicLoader::readName(const std::filesystem::path &path) { return path.stem().string(); };

    void CharacteristicLoader::parseContent(const std::filesystem::path &path, std::string &content, JFMService::CharacteristicLoader::Output &output)
    {
        auto data = loaders.at(path.extension().string())(content);
        double temperature = readTemperature(path.filename().string());
        std::string name = readName(path);
        output.data = std::move(std::make_unique<DataManagementService::CharacteristicData>(data, temperature, name));
        output.success = true;
    }
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
}
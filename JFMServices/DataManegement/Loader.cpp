#include "Loader.hpp"
namespace JFMService
{
    CharacteristicLoader::CharacteristicLoader()
    {
        loaders["dat"] = std::bind(&CharacteristicLoader::loadDatContent, this, std::placeholders::_1);
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
        auto data = loaders.at(path.extension().string())(content);
        double temperature = readTemperature(path.filename().string());
        std::string name = readName(path);
        output.data = std::move(std::make_unique<DataManagementService::CharacteristicData>(data, temperature, name));
        output.success = true;
    }
    std::vector<std::vector<double>> CharacteristicLoader::loadDatContent(std::string &content)
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
}
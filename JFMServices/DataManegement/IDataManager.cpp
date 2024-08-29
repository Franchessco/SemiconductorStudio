#include "IDataManager.hpp"
namespace JFMService::DataManagementService
{

    CharacteristicData::CharacteristicData(std::vector<std::vector<double>> &data, double temperature, const std::string &name)
        : Data{data}, Temperature{temperature}, Name{name} {}
}
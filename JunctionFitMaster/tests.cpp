#include "tests.hpp"

void Tests::test()
{
    testDataManager();
}

void Tests::testDataManager()
{
    std::cout << "test Data Manager" << std::endl;
    testLoadingCharacteristics();
}

void Tests::testLoadingCharacteristics()
{
    std::cout << "----------------------" << std::endl;
    std::cout << "Loading Characteristics" << std::endl;
    CharacteristicLoader characteristicLoader;
    JFMService::DataLoader<CharacteristicLoader> loader(characteristicLoader);

    // Assuming you have a .dat file named "example.dat" in the current directory
    std::filesystem::path path = "ivd_HZB25_T181_L0.dat";

    // JFMService::CharacteristicData data = loader.load(path);
    //  loader.load(path);

    // You can now use the loaded data, for example:

    /*std::cout << "Name: " << data.Name << "\n";
    std::cout << "Temperature: " << data.Temperature << "\n";
    std::cout << "----------------------" << std::endl;*/
}

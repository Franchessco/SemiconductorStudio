#include "tests.hpp"
#include "yaml-cpp/yaml.h"
void Tests::test()
{
    // testDataManager();
    // testModel();
    testYAML();
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
}

void Tests::testModel()
{
    NumericStorm::Fitting::Parameters<4> parameters{{1, 1e-8, 5e-5, 5e5}};
    JFMParameters fixedValues{2, 1e-9, 5e-6, 5e6};
    JFMAdditionalParameters additional(330.0, fixedValues, A | I0);
    std::vector<double> voltages;
    utils::generateVectorAtGivenRanges(voltages, 0.1, 3, 0.1);
    std::vector<double> currents(voltages);
    NumericStorm::Fitting::Data data(2);
    data[0] = voltages;
    data[1] = currents;
    FourParameterModel model;
}
struct MC
{
    std::valarray<double> parameters;
    double error;
};
void Tests::testYAML()
{
}
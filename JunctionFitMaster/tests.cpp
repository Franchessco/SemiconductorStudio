
#include "tests.hpp"
#include "yaml-cpp/yaml.h"
// #include "DataManager.hpp"
void Tests::test()
{
    testDataManager();
    // testModel();
    // testYAML();
}

void Tests::testDataManager()
{
    std::cout << "test Data Manager" << std::endl;
    testLoadingCharacteristics();
    testYAMLDumping();
    testYAMLLoading();

}

void Tests::testLoadingCharacteristics()
{
    std::cout << "----------------------" << std::endl;
    std::cout << "Loading Characteristics" << std::endl;
    DataManager manager;
    LoaderOutput loaded;
    manager.Load("ivd_HZB25_T181_L0.dat", [&](LoaderOutput output)
                 { loaded = std::move(output); });
    double T = (*loaded.data).Temperature;
    std::string name = (*loaded.data).Name;
    std::vector<double> V = (*loaded.data)[CharacteristicData::Voltage];
}

void Tests::testModel()
{
    NumericStorm::Fitting::Parameters<4> parameters{{1, 1e-8, 5e-5, 5e5}};
    JFMParameters fixedValues{2, 1e-9, 5e-6, 5e6};
    JFMAdditionalParameters additional(330.0, fixedValues, FixingConfiguration::A | FixingConfiguration::I0);
    std::vector<double> voltages;
    utils::generateVectorAtGivenRanges(voltages, 0.1, 3, 0.1);
    std::vector<double> currents(voltages);
    NumericStorm::Fitting::Data data(2);
    data[0] = voltages;
    data[1] = currents;
    FourParameterModel model;
};

void Tests::testYAMLLoading()
{

    std::cout << "----------------------" << std::endl;
    std::cout << "Loading YAML config " << std::endl;
    DataManager manager;
    LoaderOutput loaded;
    manager.Load("testDump.yaml", [&](LoaderOutput output)
                 { loaded = std::move(output); });
    MCOutput out = *(loaded.mcData);
};
void Tests::testYAMLDumping()
{
    MCOutput mcOutput;
    MCInput mcInOutput;
    FittingInput fittingInputData;
    FittingService::EstimateInput estimateData;

    estimateData.modelID = JFMModelID::Model4P;
    AdditionalParameterMap additional;
    additional[AdditionalParametersID::Temperature] = 330;
    estimateData.additionalParameters = additional;

    fittingInputData.initialData = estimateData;
    fittingInputData.name = "ExampleName";
    ParameterMap fixingMap;
    fixingMap[Fitters::ParameterID::Rs] = 5e-5;
    fixingMap[Fitters::ParameterID::Rsh] = 5e5;

    ParameterMap initialMap;
    initialMap[Fitters::ParameterID::A] = 1;
    initialMap[Fitters::ParameterID::I0] = 5e-8;
    initialMap[Fitters::ParameterID::Rs] = 5e-5;
    initialMap[Fitters::ParameterID::Rsh] = 5e5;

    ParameterMap trueMap;
    trueMap[Fitters::ParameterID::A] = 1.1;
    trueMap[Fitters::ParameterID::I0] = 5.5e-8;
    trueMap[Fitters::ParameterID::Rs] = 5.5e-5;
    trueMap[Fitters::ParameterID::Rsh] = 5.5e5;

    fittingInputData.fixConfig = fixingMap;
    fittingInputData.initialValues = initialMap;

    mcInOutput.startingData = fittingInputData;
    mcInOutput.relPath = "MCResult";

    mcInOutput.iterations = 1e4;
    mcInOutput.noise = 1;
    mcInOutput.trueParameters = trueMap;

    std::vector<MCResult> results;
    MCResult onePoint;
    std::vector<double> mcResult{2, 6e-8, 5e-6, 5e6};
    for (int i = 0; i < 3; i++)
    {
        for (const auto &[i, val] : std::views::enumerate(mcResult))
        {
            onePoint.foundParameters[(Fitters::ParameterID)i] = val;
        }
        onePoint.error = i;
        results.push_back(onePoint);
    }
    mcOutput.inputData = mcInOutput;
    mcOutput.mcResult = results;

    LoaderOutput output;
    output.mcData = std::make_unique<MCOutput>(mcOutput);
    DataManager manager;
    manager.Save("testDump.yaml",output,nullptr);
    //std::cin.get();
};
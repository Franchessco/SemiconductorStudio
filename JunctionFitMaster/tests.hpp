#pragma once
#include <DataManegement/DataManager.hpp>
#include <Models/JFMAdditionalParameters.hpp>
#include <Models/JFMParameters.hpp>
#include <Models/JFMModels.hpp>

namespace Tests
{

    using namespace JFMService;
    void test();
    void testDataManager();
    void testLoadingCharacteristics();
    void testModel();
    void testYAML();
};
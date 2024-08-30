#pragma once
#include <DataManegement/DataManager.hpp>
#include <DataManegement/Loader.hpp>
#include <DataManegement/Dumper.hpp>
#include <Models/JFMAdditionalParameters.hpp>
#include <Models/JFMParameters.hpp>
#include <Models/JFMModels.hpp>
#include "../JFMServices/pch.hpp"
namespace Tests
{
    using namespace JFMService;
    using namespace JFMService::DataManagementService;
    using namespace JFMService::Fitters;
    using namespace JFMService::FittingService;
    void test();
    void testDataManager();
    void testLoadingCharacteristics();
    void testModel();
    void testYAML();
    void testYAMLLoading();
    void testYAMLDumping();
};
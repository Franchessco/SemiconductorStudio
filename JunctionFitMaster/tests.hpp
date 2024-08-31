#pragma once
#include <DataManegement/DataManager.hpp>
#include <DataManegement/Loader.hpp>
#include <DataManegement/Dumper.hpp>
#include <Models/JFMAdditionalParameters.hpp>
#include <Models/JFMParameters.hpp>
#include <Models/JFMModels.hpp>
#include "../JFMServices/pch.hpp"
#include <../JFMServices/Fitting/PreFitter.hpp>
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
    void testFitting();
    void testSixParameterModel();
    void testFourParameterModel();
    void calculateData();
    //! testing fitting
    void createData(CalculatingData &data);
    EstimateInput createEstimateInput(const CalculatingData &calculatingDataInput);
    FittingInput createFittingInput(const EstimateInput &estimateInput);
    
    //! testing auto range
    void testAutoRange();
    

};
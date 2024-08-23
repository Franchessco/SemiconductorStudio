#include "CalculateData.hpp"
#include "JFMModels.hpp"
void JFMService::DataCalculator::CalculateData(CalculatingData &input)
{
    FourParameterModel model4;
    SixParameterModel model6;

    switch (input.modelID)
    {
    case Model4P:
        model4.call(input);
        break;
    case Model6P:
        model6.call(input);
        break;
    default:
        break;
    }
}
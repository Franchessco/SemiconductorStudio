#include "JFMFitter.hpp"
#include "../NumericStorm.hpp"
#include "../Models/JFMModels.hpp"
#include "FittingSetup.hpp"
namespace JFMService
{

    FourParameterFitter::FourParameterFitter()
    {
    }

    void FourParameterFitter::reinItialize()
    {
    }

    void FourParameterFitter::Fit(const FittingInput &input, Callback callback)
    {
        }
    void Fitter::Fit(const FittingInput &input, Callback callback) { fitterMap[input.initialData.modelID]->Fit(input, callback); }
}
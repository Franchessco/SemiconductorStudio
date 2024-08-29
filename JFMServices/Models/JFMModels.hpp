#pragma once
#include "pch.hpp"

#include "../Fitting/JFMIFitting.hpp"
#include "JFMParameters.hpp"
#include "JFMAdditionalParameters.hpp"
#include "../Vendor/LambertW/LambertW.h"
#include "Fitting.hpp" // numeric storm

namespace JFMService
{

    using namespace FittingService;
    using namespace NumericStorm::Fitting;

    class FourParameterModel : public ModelBase<4, JFMAdditionalParameters>
    {
    public:
        FourParameterModel();
        void current(Data &data, const NumericStorm::Fitting::Parameters<4> &parameters, const JFMAdditionalParameters &additionalParameters);
        void call(const CalculatingData& data);
    };

    class SixParameterModel : public ModelBase<6, JFMAdditionalParameters>
    {
    public:
        SixParameterModel();
        void current(Data &data, const NumericStorm::Fitting::Parameters<6> &parameters, const JFMAdditionalParameters &additionalParameters);
        void call(const CalculatingData& data);
    };
}
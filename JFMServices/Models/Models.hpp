#pragma once
#include "pch.hpp"
#include "ModelBase.hpp"
#include "IFitting.hpp"
#include "JFMParameters.hpp"
#include "JFMAdditionalParameters.hpp"
#include "LambertW.h"

namespace JFMService
{

    using namespace FittingService;
    using namespace NumericStorm::Fitting;
    class Model
    {
    public:
        virtual void operator()(JFMParameters &parameters, PlotData &data, const JFMAdditionalParameters &additionalParameters) = 0;

    protected:
        std::unordered_map<ModelID, std::function<void>(JFMParameters &parameters, PlotData &data, const JFMAdditionalParameters &additionalParameters)> m_models;
    };

    class FourParameterModel : public ModelBase<4, JFMAdditionalParameters>, Model
    {
        FourParameterModel(ModelID id);

    private:
        void withoutRs(JFMParameters &parameters, PlotData &data, const JFMAdditionalParameters &additionalParameters);
        void withoutRsh(JFMParameters &parameters, PlotData &data, const JFMAdditionalParameters &additionalParameters);
        void full(JFMParameters &parameters, PlotData &data, const JFMAdditionalParameters &additionalParameters);
    };
    class SixParameterModel : public ModelBase<6, JFMAdditionalParameters>, Model
    {
        SixParameterModel(ModelID id);

    private:
        void withoutRs(JFMParameters &parameters, PlotData &data, const JFMAdditionalParameters &additionalParameters);
        void withoutRsh(JFMParameters &parameters, PlotData &data, const JFMAdditionalParameters &additionalParameters);
        void full(JFMParameters &parameters, PlotData &data, const JFMAdditionalParameters &additionalParameters);
    };
}
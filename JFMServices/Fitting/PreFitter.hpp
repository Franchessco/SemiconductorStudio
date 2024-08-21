#pragma once
#include "pch.hpp"
#include "IFitting.hpp"
namespace JFMService
{
    class IPreFit
    {
    public:
        std::pair<size_t, size_t> RangeData(const FittingService::PlotData &characteristic);
        virtual FittingService::ParameterMap Estimate(const FittingService::EstimateInput &input) = 0;

    private:
        size_t getLowerRange(const FittingService::PlotData &characteristic);
        size_t getUpperRange(const FittingService::PlotData &characteristic);
    };

    class FourParameterModelPreFit : IPreFit
    {

        virtual FittingService::ParameterMap Estimate(const FittingService::EstimateInput &input) override;
    };
}

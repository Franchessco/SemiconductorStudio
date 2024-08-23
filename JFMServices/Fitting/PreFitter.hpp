#pragma once
#include "pch.hpp"
#include "JFMIFitting.hpp"
namespace JFMService
{
    class AbstractPreFit
    {
    public:
        std::pair<size_t, size_t> RangeData(const FittingService::PlotData &characteristic);
        virtual FittingService::ParameterMap Estimate(const FittingService::EstimateInput &input) = 0;

    private:
        size_t getLowerRange(const FittingService::PlotData &characteristic);
        size_t getUpperRange(const FittingService::PlotData &characteristic);
    };

    class FourParameterModelPreFit : AbstractPreFit
    {
        virtual FittingService::ParameterMap Estimate(const FittingService::EstimateInput &input) override;
    };

    class PreFitter
    {
        using PreFitterMap = std::unordered_map<std::string, std::shared_ptr<AbstractPreFit>>;

    public:
        PreFitter() = default;
        std::pair<size_t, size_t> RangeData(const FittingService::PlotData &characteristic);
        virtual FittingService::ParameterMap Estimate(const FittingService::EstimateInput &input);

    private:
        PreFitterMap preFitterMap{};
    };
}

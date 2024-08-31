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
    public:
        FourParameterModelPreFit() = default;
        virtual FittingService::ParameterMap Estimate(const FittingService::EstimateInput &input) override;
        std::pair<size_t, size_t> rangeData(const FittingService::PlotData& characteristic) { return this->RangeData(characteristic); };
    };

    class PreFitter
    {
        using PreFitterMap = std::unordered_map<std::string, std::shared_ptr<AbstractPreFit>>;

    public:
        //PreFitter() = default;
        PreFitter() 
        {
            //preFitterMap["f"] = std::make_shared<FourParameterModelPreFit>();
        };
        std::pair<size_t, size_t> RangeData(const FittingService::PlotData& characteristic) 
        {
            return preFitterMap["f"]->RangeData(characteristic); 
        };
        virtual FittingService::ParameterMap Estimate(const FittingService::EstimateInput& input) { return FittingService::ParameterMap(); };

    private:
        PreFitterMap preFitterMap{};
    };
}

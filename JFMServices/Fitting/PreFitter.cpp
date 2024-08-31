#include "PreFitter.hpp"
namespace JFMService
{

    std::pair<size_t, size_t> AbstractPreFit::RangeData(const FittingService::PlotData &characteristic)
    {
        return std::make_pair<size_t, size_t>(getLowerRange(characteristic), getUpperRange(characteristic));
    }; 
    size_t AbstractPreFit::getLowerRange(const FittingService::PlotData& characteristic)
    {
        size_t start{ 0 };
        for (const auto& [V, I] : std::views::zip(characteristic.voltageData, characteristic.voltageData))
            if (I < 0.0 or V < 0.0)
                start++;
        return start;

    };

    size_t AbstractPreFit::getUpperRange(const FittingService::PlotData &characteristic)
    {

        std::vector<double> copy = { characteristic.currentData.begin(), characteristic.currentData.end() };
        std::reverse(copy.begin(), copy.end());

        auto avg = [&](int first, int window)
        {
            int last = std::min(first + window, static_cast<int>(copy.size()));
            if (first >= last || first < 0) 
                return 0.0; 
            double sum = std::accumulate(copy.begin() + first, copy.begin() + last, 0.0);
            return sum / (last - first); 
           };

        size_t index = 0;
        int window = 5;
        
        for (const auto& [i, item] : std::views::enumerate(copy))
        {
            if ((item - avg(i, window)) < 0.0001)
                continue;
            else
                return copy.size() - i - window
        }

        return copy.size();
    };
    FittingService::ParameterMap JFMService::FourParameterModelPreFit::Estimate(const FittingService::EstimateInput &input)
    {
        return FittingService::ParameterMap();
    }
}
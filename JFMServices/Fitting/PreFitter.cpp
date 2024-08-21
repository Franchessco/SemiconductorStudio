#include "PreFitter.hpp"
namespace JFMService
{

    std::pair<size_t, size_t> IPreFit::RangeData(const FittingService::PlotData &characteristic)
    {
        return std::make_pair<size_t, size_t>(getLowerRange(characteristic), getUpperRange(characteristic));
    };
    size_t IPreFit::getLowerRange(const FittingService::PlotData &characteristic)
    {
        size_t start{0};
        for (const auto &[V, I] : std::views::zip(characteristic.voltageData, characteristic.voltageData))
            if (I < 0.0 or V < 0.0)
                start++;
        return start;
    };
    size_t IPreFit::getUpperRange(const FittingService::PlotData &characteristic)
    {
        double sum{0.0};
        std::span<double> logged{characteristic.currentData};
        std::ranges::transform(logged, logged, [](double item)
                               { return std::log(item); });

        std::span<double> averaged{characteristic.currentData};
        int N = 4;
        for (size_t i = 0; i < averaged.size(); i++)
        {
            sum += logged[i];
            if (i >= N)
                sum -= logged[i - N];

            averaged[i] = sum / std::min(i + 1, (size_t)N);
        }
        std::span<double> expped{logged};
        std::ranges::transform(logged, expped, [](double item)
                               { return std::exp(item); });

        std::pair<std::vector<double>, std::vector<double>> derivativeValues;
        auto derivate = [&](const std::span<double> &v, const std::span<double> &c, std::pair<std::vector<double>, std::vector<double>> &result)
        {
            double der{0.0};

            result.first.clear();
            result.second.clear();

            for (size_t i = 0; i < c.size() - 2; i++)
            {
                der = (c[i + 2] - c[i]) / (v[i + 2] - v[i]);
                result.first.push_back(v[i + 1]);
                result.second.push_back(der);
            }
        };

        derivate(characteristic.voltageData, logged, derivativeValues);
        size_t ind{derivativeValues.second.size() - 1};
        double avg{0.0};
        do
        {
            avg *= ind;
            avg += derivativeValues.second[ind];
            avg /= ++ind;
        } while (derivativeValues.second[ind + 1] / avg < 5.0);

        return ind;
    };
    FittingService::ParameterMap JFMService::FourParameterModelPreFit::Estimate(const FittingService::EstimateInput &input)
    {
        return FittingService::ParameterMap();
    }
}
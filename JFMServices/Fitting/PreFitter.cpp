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
     /*
        size_t start{0};
        for (const auto &[V, I] : std::views::zip(characteristic.voltageData, characteristic.voltageData))
            if (I < 0.0 or V < 0.0)
                start++;
   
        std::vector<double> logI{};
        double sum = 0;
        std::transform(characteristic.currentData.begin() + start, characteristic.currentData.end(), std::back_inserter(logI), [](double i) {return std::log(i); });

        
        std::vector<double> V, I;

        V.resize(characteristic.voltageData.size() - start);
        std::copy(characteristic.voltageData.begin() + start, characteristic.voltageData.end(), V.begin());
        I.resize(characteristic.voltageData.size() - start);
        const unsigned int N{ 4 };

        for (size_t i = 0; i < I.size(); i++) {
            sum += logI[i];
            if (i >= N) {
                sum -= logI[i - N];
            }
            I[i] = sum / std::min(i + 1, (size_t)N);
        }


        std::transform(I.begin(), I.end(), I.begin(), [&](double i) {return std::exp(i); });


        auto derivate = [&](const std::vector<double>& v, const std::vector<double>& c, std::pair<std::vector<double>, std::vector<double>>& result) {
            double der{ 0.0 };

            result.first.clear();
            result.second.clear();

            for (size_t i = 0; i < c.size() - 2; i++) {
                der = (c[i + 2] - c[i]) / (v[i + 2] - v[i]);
                result.first.push_back(v[i + 1]);
                result.second.push_back(der);
            }
            };
        std::pair<std::vector<double>, std::vector<double>>ADerivative;
        derivate(V, logI, ADerivative);

        this is already working, can we do better?
        size_t index{0};
        auto avg = [&](int first,int second)
        {
                return std::accumulate(ADerivative.second.begin()+first, ADerivative.second.begin()+first+second,0);
        };
        //std::reverse(ADerivative.second.begin(), ADerivative.second.end());
        int window = 5;
        for (const auto&[i,item]:std::views::enumerate(ADerivative.second))
        {
            if (avg(i,window) >1)
            {
                index += 1;
                std::cout << "index: " << index << " item: " << item <<" avg(i,5): "<< avg(i, window) << std::endl;
            }
            else
            {
                //int j = i + 1;
                std::cout << "index: " << index++ << " item: " << ADerivative.second[index] << " avg(i,5): " << avg(i, window) << std::endl;
                return index;
            }

        }
        */

        std::vector<double> copy = { characteristic.currentData.begin(), characteristic.currentData.end() };
        std::reverse(copy.begin(), copy.end());

        auto avg = [&](int first, int window) -> double
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
            {
                std::cout << "index: " << i << " item: " << item << " avg(i,5): " << avg(i, window) <<" (item - avg(i, window)): "<< (item - avg(i, window)) << std::endl;
                continue;
            }
            else
            {

                std::cout << "index: " << i << " item: " << copy[i] << " avg(i,5): " << avg(i, window) << std::endl;
                std::cout << "copy.size() - index - window " << copy.size() - i  - window << std::endl;
                return copy.size() - i - window;
            }
        }

        return copy.size();
    };
    FittingService::ParameterMap JFMService::FourParameterModelPreFit::Estimate(const FittingService::EstimateInput &input)
    {
        return FittingService::ParameterMap();
    }
}
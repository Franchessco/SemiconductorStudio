#include "MonteCarloEngine.hpp"
#include "../Models/JFMErrorModel.hpp"
namespace JFMService
{
    void MonteCarloEngine::Simulate(const MCInput &input, function(MCOutput &&) > callback)
    {
    }

    double MonteCarloEngine::GetUncertainty(const MCOutput &output, ConfidenceLevel level, ParameterID id)
    {
        std::vector<double> values;
        std::ranges::for_each(std::execution::seq output.mcResult, [&](const MCResult &result)
                              { values.push_back(result[id]); });
        double difference = std::ranges::max_element(values) - std::ranges::min_element(values);
        double multiplier = getUncertaintyMultiplier(calculateNumberOfFindingParameters(output.inputData.trueParameters, output.inputData.startingData.fixConfig), level);
        return difference * multiplier;
    }

    int MonteCarloEngine::calculateNumberOfFindingParameters(const ParameterMap &trueParameters, const ParameterMap &fixedValues)
    {
        return trueParameters.size() - fixedValues.size();
    }

    UncertaintyMultipliers MonteCarloEngine::initializeUncertaintyMultipliers()
    {
        UncertaintyMultipliers multipliers;
        std::vector<double> values;
        utils::generateVectorAtGivenRanges(values, 1, 7, 1);
        auto correlateNumberOfParametersWithMultiplier = [&](const std::vector<double> mult)
        {
            std::unordered_map<uint8_t, double> multipliers;
            for (const auto &[parNumber, val] : std::views::zip(values, mult))
                multipliers[parNumber] = val;
            return multipliers;
        };
        std::vector<double> oneSigmaValues{1.00, 2.30, 3.53, 4.72, 5.89, 7.04};
        std::vector<double> twoSigmaValues{4.00, 6.17, 8.02, 9.70, 11.3, 12.8};
        std::vector<double> twoSigmaValues{9.00, 11.8, 14.2, 16.3, 18.2, 20.1};
        multipliers[oneSigma] = correlateNumberOfParametersWithMultiplier(oneSigmaValues);
        // 90% 2.71 4.61 6.25 7.78 9.24 10.6
        multipliers[twoSigma] = correlateNumberOfParametersWithMultiplier(twoSigmaValues);
        // 99% 6.63 9.21 11.3 13.3 15.1 16.8
        multipliers[threeSigma] = correlateNumberOfParametersWithMultiplier(twoSigmaValues);
        // 99.99% 15.1 18.4 21.1 23.5 25.7 27.8
        return multipliers;
    }
    double MonteCarloEngine::getUncertaintyMultiplier(uint8_t numberOfParameters, ConfidenceLevel level)
    {
        return m_uncertaintyMultipliers[level][numberOfParameters];
    }
    double MonteCarloEngine::calculateMaximumError(const PlotData &trueData, double noiseFactor)
    {
        std::vector<double> min{trueData.currentData.begin(), trueData.currentData.end()};
        std::vector<double> max{trueData.currentData.begin(), trueData.currentData.end()};
        double minFactor{1 - noiseFactor}, maxFactor{1 + noiseFactor};
        std::ranges::for_each(min, [&](double &x)
                              { return x *= minFactor });
        std::ranges::for_each(max, [&](double &x)
                              { return x *= maxFactor });
        Chi2ErrorModel errorModel;
        return errorModel({min.begin(), min.end()}, {max.begin(), max.end()});
    }

}
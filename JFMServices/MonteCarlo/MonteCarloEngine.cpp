#include "MonteCarloEngine.hpp"
#include "../Models/JFMErrorModel.hpp"
namespace JFMService
{
    void MonteCarloEngine::Simulate(const MCInput &input, std::function<void(MCOutput &&)> callback)
    {
    }

    double MonteCarloEngine::GetUncertainty(const MCOutput &output, ConfidenceLevel level, ParameterID id)
    {
        std::vector<double> values;
        values.reserve(output.mcResult.size());

        std::ranges::transform(output.mcResult, std::back_inserter(values), [&](const MCResult &result)
                               { return result.foundParameters.at(id); });

        if (values.size() < 2)
        {
            throw std::invalid_argument("Insufficient data to calculate uncertainty");
        }

        double max_value = *std::ranges::max_element(values);
        double min_value = *std::ranges::min_element(values);
        double difference = max_value - min_value;

        double multiplier = getUncertaintyMultiplier(
            calculateNumberOfFindingParameters(output.inputData.trueParameters,
                                               output.inputData.startingData.fixConfig),
            level);

        return difference * multiplier;
    }

    int MonteCarloEngine::calculateNumberOfFindingParameters(const ParameterMap &trueParameters, const ParameterMap &fixedValues)
    {
        return trueParameters.size() - fixedValues.size();
    }

    MonteCarloEngine::UncertaintyMultipliers MonteCarloEngine::initializeUncertaintyMultipliers()
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
        std::vector<double> threeSigmaValues{9.00, 11.8, 14.2, 16.3, 18.2, 20.1};
        multipliers[oneSigma] = correlateNumberOfParametersWithMultiplier(oneSigmaValues);
        // 90% 2.71 4.61 6.25 7.78 9.24 10.6
        multipliers[twoSigma] = correlateNumberOfParametersWithMultiplier(twoSigmaValues);
        // 99% 6.63 9.21 11.3 13.3 15.1 16.8
        multipliers[threeSigma] = correlateNumberOfParametersWithMultiplier(threeSigmaValues);
        // 99.99% 15.1 18.4 21.1 23.5 25.7 27.8
        return multipliers;
    }
    double MonteCarloEngine::getUncertaintyMultiplier(uint8_t numberOfParameters, ConfidenceLevel level)
    {
        return m_uncertaintyMultipliers.at(level).at(numberOfParameters);
    }
    double MonteCarloEngine::calculateMaximumError(const PlotData &trueData, double noiseFactor)
    {
        std::vector<double> min{trueData.currentData.begin(), trueData.currentData.end()};
        std::vector<double> max{trueData.currentData.begin(), trueData.currentData.end()};
        double minFactor{1 - noiseFactor}, maxFactor{1 + noiseFactor};
        std::ranges::for_each(min, [&](double &x)
                              { return x *= minFactor; });
        std::ranges::for_each(max, [&](double &x)
                              { return x *= maxFactor; });
        Chi2ErrorModel errorModel;
        double error = errorModel.CalculateError({ min.begin(), min.end() }, { max.begin(), max.end() });
        return error;
    }

}
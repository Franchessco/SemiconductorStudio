#pragma once
#include "../pch.hpp"
#include "../Fitting/JFMIFitting.hpp"
#include "../Fitting/JFMFitter.hpp"

namespace JFMService
{
    using namespace FittingService;
    enum ConfidenceLevel
    {
        oneSigma = 0,
        twoSigma,
        threeSigma
    };
    class MonteCarloEngine
    {
    public:
        MonteCarloEngine();
        void Simulate(const MCInput &input, std::function<void>(MCOutput &&) > callback);
        double GetUncertainty(const MCOutput &output, ConfidenceLevel level, ParameterID id);

    private:
        using UncertaintyMultipliers = std::unordered_map<uint8_t, std::unordered_map<uint8_t, double>>;
        const UncertaintyMultipliers m_uncertaintyMultipliers;

        Fitter m_fitter;

    private:
        int calculateNumberOfFindingParameters(const ParameterMap &trueParameters, const ParameterMap &fixedValues);
        UncertaintyMultipliers initializeUncertaintyMultipliers();
        double getUncertaintyMultiplier(uint8_t numberOfParameters, ConfidenceLevel level);

        double calculateMaximumError(const PlotData &trueData, double noiseFactor);
    };
}
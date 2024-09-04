#pragma once
#include "../pch.hpp"
#include "../Fitting/JFMIFitting.hpp"
#include "../Fitting/JFMFitter.hpp"
#include "../Fitting/PreFitter.hpp"


namespace JFMService
{
    using namespace FittingService;
    enum ConfidenceLevel : uint8_t
    {
        oneSigma = 0,
        twoSigma,
        threeSigma
    };
    class MonteCarloEngine
    {
    public:
        MonteCarloEngine();
        void Simulate(const MCInput &input, std::function<void(MCOutput &&)> callback);
        double GetUncertainty(const MCOutput &output, int level, ParameterID id);

    private:
        using UncertaintyMultipliers = std::unordered_map<uint8_t, std::unordered_map<uint8_t, double>>;
        const UncertaintyMultipliers m_uncertaintyMultipliers;

        Fitters::Fitter m_fitter;
        PreFitter m_prefitter;

        //inline thread_local static std::mt19937 m_generator;
        inline static thread_local std::mt19937 m_generator{std::random_device{}()};

    private:
        int calculateNumberOfFindingParameters(const ParameterMap &trueParameters, const ParameterMap &fixedValues);
        UncertaintyMultipliers initializeUncertaintyMultipliers();
        double getUncertaintyMultiplier(uint8_t numberOfParameters, ConfidenceLevel level);

        double calculateMaximumError(const PlotData &trueData, double noiseFactor);
        void generateNoise(double &value, double factor);
        void simulate(const std::shared_ptr<AbstractPreFit>& preFitter , const std::shared_ptr<Fitters::AbstractFitter> fitter, MCInput& input, std::vector<MCResult>& results, int i);
        void calculateFittingError(const MCInput &input, MCResult &result);
    };
}
#pragma once
#include "../Fitting/JFMIFitting.hpp"
#include "DataManager.hpp"
#include "../Fitting/PreFitter.hpp"
#include "../Fitting/JFMFitter.hpp"
#include "../Models/CalculateData.hpp"
namespace JFMService::FittingService
{
    using namespace DataManagementService;

    class Fitting : public IFitting
    {
    public:
        Fitting();
        virtual NumericsConfig GetConfiguration() override;
        virtual void CalculateData(CalculatingData &input) override;
        virtual double CalculateError(const std::span<double> &original, const std::span<double> &checked) override;

        virtual std::pair<size_t, size_t> RangeData(const PlotData &characteristic) override;
        virtual std::unordered_map<ParameterID, double> Estimate(const EstimateInput &input) override;
        virtual void Fit(const FittingInput &input, std::function<void(ParameterMap &&)> callback) override;

        virtual void Simulate(const MCInput &input, std::function<void(MCOutput &&)> callback) override;

    private:
        DataManager m_dataManager;
        PreFitter m_preFitter;
        Fitters::Fitter m_fitter;
        DataCalculator m_dataCalculator;
        const NumericsConfig m_numericsConfig;

    private:
        NumericsConfig instantiateNumericsConfig();
    };
}
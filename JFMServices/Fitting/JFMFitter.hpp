#pragma once
#include "pch.hpp"
#include "../Fitting/JFMIFitting.hpp"
#include "../Models/JFMAdditionalParameters.hpp"

namespace JFMService::Fitters
{

    enum ParameterID
    {
        A = 0,
        I0,
        Rs,
        Rsh,
        alpha,
        Rsh2,
        size
    };
    enum AdditionalParametersID
    {
        Temperature = size,
    };

    using namespace FittingService;
    using Callback = std::function<void(ParameterMap &&)>;

    class AbstractFitter
    {
    public:
        virtual void Fit(const FittingInput &input, Callback callback) = 0;

        // protected:
        //     template <size_t size>
        //     IVFittingSetup<size> transferFittingSetUp(const FittingInput &input);
        //     Data transferFittingData(const PlotData &input);
        //     JFMAdditionalParameters transferAdditionalParameters(const AdditionalParameterMap &input, const ParameterMap &fixingConfig);
        //     template <size_t size>
        //     NumericStorm::Fitting::Parameters<size> transferInitialPoint(const ParameterMap &initial);
        //     template <size_t size>
        //     SimplexOptimizationResults<size> fit(const IVFittingSetup<size> &setUp, const NumericStorm::Fitting::Parameters<size> &initialPoint, const Data &data, const AdditionalParameters &additionalParameters);
    };

    class FourParameterFitter : public AbstractFitter
    {
    public:
        FourParameterFitter() = default;
        virtual void Fit(const FittingInput &input, Callback callback) override;
    };
    class SixParameterFitter : public AbstractFitter
    {
    public:
        SixParameterFitter() = default;
        virtual void Fit(const FittingInput &input, Callback callback) override;
    };

    class Fitter
    {
        using FitterMap = std::unordered_map<ModelID, std::shared_ptr<AbstractFitter>>;

    public:
        Fitter();
        void Fit(const FittingInput &input, Callback callback);

    private:
        FitterMap fitterMap{};
    };

}
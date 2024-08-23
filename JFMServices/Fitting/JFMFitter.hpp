#pragma once
#include "pch.hpp"
#include "../Fitting/JFMIFitting.hpp"
#include "../Models/JFMAdditionalParameters.hpp"

namespace JFMService
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

    protected:
        template <size_t size>
        std::array<IVFittingSetup<size>, 3> transferFittingSetUp(const FittingInput &input);
        std::array<Data, 3> transferFittingData(const PlotData &input);
        std::array<JFMAdditionalParameters, 3> transferAdditionalParameters(const AdditionalParameterMap &input, const ParameterMap &fixingConfig);
    };

    class FourParameterFitter : public AbstractFitter
    {
    public:
        FourParameterFitter();
        virtual void Fit(const FittingInput &input, Callback callback) override;
    };
    class SixParameterFitter : public AbstractFitter
    {
    };

    class Fitter
    {
        using FitterMap = std::unordered_map<ModelID, std::shared_ptr<AbstractFitter>>;

    public:
        Fitter() = default;
        void Fit(const FittingInput &input, Callback callback);

    private:
        FitterMap fitterMap{};
    };

}
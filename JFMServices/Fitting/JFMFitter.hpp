#pragma once
#include "pch.hpp"
#include "../Fitting/JFMIFitting.hpp"

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
        Temperature = size
    };

    using namespace FittingService;
    using Callback = std::function<void(ParameterMap &&)>;

    class AbstractFitter
    {
    public:
        virtual void Fit(const FittingInput &input, Callback callback) = 0;

    protected:
    protected:
        virtual void reinItialize() = 0;
    };

    class FourParameterFitter : public AbstractFitter
    {
    public:
        FourParameterFitter();
        virtual void reinItialize();
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
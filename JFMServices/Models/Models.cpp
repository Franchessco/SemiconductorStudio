#include "Models.hpp"

void JFMService::FourParameterModel::withoutRs(JFMParameters &parameters, PlotData &data, const JFMAdditionalParameters &additionalParameters)
{
    // I(V) = I0(e^(qV/AKT)-1) - V/Rsh
    auto [A, I0, Rs, Rch] = utils::cast<4>(parameters.getParameters());
    const double k = 8.6e-5;

    auto func = [&](double &V, double &I, double &I0, double &A, double &Rsh, double T)
    {
        double x = std::exp(V / (A * k * T));
        I = I0 * (x - 1);
    };

    for (const auto &[V, I] : std::views::zip(data.voltageData, data.voltageData))
        func(V, I, I0, A, Rch, additionalParameters.Temperature);
}

void JFMService::FourParameterModel::withoutRsh(JFMParameters &parameters, PlotData &data, const JFMAdditionalParameters &additionalParameters)
{
    // I(V) = I0(e^(q(V-IRs)/AKT)-1)
    auto [A, I0, Rs, Rch] = utils::cast<4>(parameters.getParameters());
    const double k = 8.6e-5;

    auto func = [&](double &V, double &I, double &I0, double &A, double &Rs, double T)
    {
        double x = ((I0 * Rs) / (A * k * T)) * std::exp(V / (A * k * T));
        double I_lw = utl::LambertW<0>(x);
        I_lw *= (A * k * T) / Rs;
        I = I_lw + (V - I_lw * Rs);
    };

    for (const auto &[V, I] : std::views::zip(data.voltageData, data.voltageData))
        func(V, I, I0, A, Rs, additionalParameters.Temperature);
}

void JFMService::FourParameterModel::full(JFMParameters &parameters, PlotData &data, const JFMAdditionalParameters &additionalParameters)
{
    // I(V) = I0(e^(q(V-IRs)/AKT)-1) - (V - IRs)/Rsh
    auto [A, I0, Rs, Rch] = utils::cast<4>(parameters.getParameters());
    const double k = 8.6e-5;

    auto func = [&](double &V, double &I, double &I0, double &A, double &Rsh, double &Rs, double T)
    {
        double x = ((I0 * Rs) / (A * k * T)) * std::exp(V / (A * k * T));
        double I_lw = utl::LambertW<0>(x);
        I_lw *= (A * k * T) / Rs;
        I = I_lw + (V - I_lw * Rs) / Rsh;
    };

    for (const auto &[V, I] : std::views::zip(data.voltageData, data.voltageData))
        func(V, I, I0, A, Rch, Rs, additionalParameters.Temperature);
}

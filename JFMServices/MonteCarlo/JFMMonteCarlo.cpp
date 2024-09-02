#include "JFMMonteCarlo.hpp"

void JFMService::MonteCarlo::Simulate(const MCInput &input, std::function<void(MCOutput &&)> callback)
{
    engine.Simulate(input, callback);
}

double JFMService::MonteCarlo::GetUncertainty(const MCOutput &output, ConfidenceLevel level, ParameterID id)
{
    return engine.GetUncertainty(output, level, id);
}

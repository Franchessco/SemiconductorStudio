#include "JFMAdditionalParameters.hpp"
namespace JFMService 
{
JFMAdditionalParameters::JFMAdditionalParameters(double temperature, JFMParameters fixingValues, FixingConfiguration configuration)
    : Temperature(temperature ), fixingValues(fixingValues), fixingConfiguration(configuration) {};
}
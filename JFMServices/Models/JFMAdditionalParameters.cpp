#include "JFMAdditionalParameters.hpp"

JFMService::JFMAdditionalParameters::JFMAdditionalParameters(double Temperature, JFMParameters fixingValues, FixingConfiguration configuration)
    : m_Temperature{Temperature}, m_fixingValues{fixingValues}, m_fixingConfiguration{configuration}
{
}
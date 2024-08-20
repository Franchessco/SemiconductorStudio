#include "AdditionalParameters.hpp"

JFMService::JFMAdditionalParameters::JFMAdditionalParameters(double Temperature, Parameters fixingValues, FixingConfiguration configuration)
    : m_Temperature{Temperature}, m_fixingValues{fixingValues}, m_fixingConfiguration{configuration}
{
}
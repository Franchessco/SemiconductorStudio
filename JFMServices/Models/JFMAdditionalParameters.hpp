#pragma once
#include "pch.hpp"
#include "utils.hpp"
#include "JFMParameters.hpp"
namespace JFMService
{
    enum FixingConfiguration
    {

        A = BIT(1),
        I0 = BIT(2),
        Rs = BIT(3),
        Rsh = BIT(4),
        alpha = BIT(5),
        Rsh2 = BIT(6),

    };

    class JFMAdditionalParameters
    {
    public:
        JFMAdditionalParameters() = default;
        JFMAdditionalParameters(double Temperature, JFMParameters fixingValues, FixingConfiguration configuration);
        double Temperature{-1};

    private:
        JFMParameters m_fixingValues;
        FixingConfiguration m_fixingConfiguration;
    };
}
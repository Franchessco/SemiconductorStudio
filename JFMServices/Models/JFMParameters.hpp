#pragma once
#include "pch.hpp"

namespace JFMService
{
    class JFMParameters
    {
    public:
        JFMParameters(std::initializer_list<double> parameters)
            : m_parameters{parameters} {};
        std::valarray<double> getParameters() { return m_parameters; };

    private:
        std::valarray<double> m_parameters;
    };
}
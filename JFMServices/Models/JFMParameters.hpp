#pragma once
#include "pch.hpp"

namespace JFMService
{
    class JFMParameters
    {
    public:
        JFMParameters() = default;
        JFMParameters(std::initializer_list<double> parameters)
            : m_parameters{parameters} {};
        std::valarray<double>& getParameters() { return m_parameters; };
        double &operator[](int index) { return m_parameters[index]; };

    private:
        std::valarray<double> m_parameters;
    };
}
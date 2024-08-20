#pragma once
#include "pch.hpp"

namespace JFMService
{
    class Parameters
    {
    public:
        Parameters(std::initializer_list<double> parameters)
            : m_parameters{parameters} {};

    private:
        std::valarray<double> m_parameters;
    };
}
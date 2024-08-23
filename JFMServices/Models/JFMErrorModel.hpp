#pragma once
#include "pch.hpp"
#include "../Fitting/JFMIFitting.hpp"
#include <../NumericStorm.hpp>
namespace JFMService
{
    using namespace FittingService;
    using namespace NumericStorm::Fitting;
    class Chi2ErrorModel : public ErrorModel
    {
    public:
        Chi2ErrorModel();
        double CalculateError(const std::span<double> &original, const std::span<double> &checked);

    private:
        double NSCalculateError(const Data &original, const Data &checked);
    };

}
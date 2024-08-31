#include "JFMErrorModel.hpp"
namespace JFMService
{
    Chi2ErrorModel::Chi2ErrorModel()
    {
        using namespace std::placeholders;
        m_errorModel = std::bind(&Chi2ErrorModel::NSCalculateError, this, _1, _2);
    }

    double Chi2ErrorModel::NSCalculateError(const Data &original, const Data &checked)
    {
        double error{0.0};
        for (const auto &[orig, check] : std::views::zip(original[1], checked[1]))
            error += std::pow((std::log(orig) - std::log(check)), 2) ;
        return error;
    }

    double Chi2ErrorModel::CalculateError(const std::span<double> &original, const std::span<double> &checked)
    {
        Data origin, check;
        origin[0] = std::vector<double>(original.begin(), original.end());
        check[0] = std::vector<double>(checked.begin(), checked.end());
        return NSCalculateError(origin, check);
    }

}
//
//diff = log(data[, 2]) - log(jv[, 2])
//sum(diff ^ 2 / log(data[, 2]) ^ 2)
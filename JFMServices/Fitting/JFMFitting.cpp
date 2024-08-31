#include "JFMFitting.hpp"
#include "CalculateData.hpp"
#include "IDataManager.hpp"
namespace JFMService::FittingService
{
#define ID_TO_NAME(ID) return #ID

    Fitting::Fitting()
        : m_numericsConfig(instantiateNumericsConfig())
    {
    }
    NumericsConfig Fitting::instantiateNumericsConfig()
    {
        NumericsConfig config;
        auto intRange = [&](double min, double max, double step)
        {
            int size = static_cast<int>((max - min) / step) + 1;
            std::vector<unsigned int> destination(size);

            int count = -1;
            std::ranges::generate(destination.begin(), destination.end(), [&]()
                                  { count++; return min + count * step; });
            return destination;
        };
        auto createModelMap = [&]()
        {
            auto ModelsToParameters = [](int id)
            {
                switch (id)
                {
                case Model4P:
                    return "FourParameterModel";
                case Model6P:
                    return "SixParameterModel";

                default:
                    break;
                }
            };
            ModelParameters map;
            map[Model4P] = intRange(0, Model4P, 1);
            map[Model6P] = intRange(0, Model6P, 1);
            return map;
        };
        auto createParameterMap = [&]()
        {
            JFMService::FittingService::Parameters map;
            std::string name;
            for (auto index : std::ranges::iota_view(0, Fitters::ParameterID::p_size))
                map[index] = DataManagementService::parameterIdToString((Fitters::ParameterID)index);
            return map;
        };
        auto createModelsParameterMap = [&]()
        {
            ModelParameters map;
            map[Model4P] = intRange(0, Model4P, 1);
            map[Model6P] = intRange(0, Model6P, 1);
            return map;
        };
        auto createGeneralBounds = [&]()
        {
            // todo adjust proper values for alpha and Rsh2
            //! order A,I0,Rs,Rsh,alpha,Rsh2
            std::vector<double> minBound{0.5, 1e-20, 1e-7, 10, 0, 0};
            std::vector<double> maxBound{20, 1e-3, 1e2, 1e9, 10, 1e-10};
            ParamBounds bounds;
            for (const auto &[id, min, max] : std::views::zip(std::ranges::iota_view(0, Fitters::ParameterID::p_size), minBound, maxBound))
                bounds[id] = std::make_pair(min, max);
            return bounds;
        };

        config.modelParameters = createModelMap();
        config.parameters = createParameterMap();
        config.modelParameters = createModelsParameterMap();
        config.paramBounds = createGeneralBounds();
        return config;
    };

    void Fitting::Fit(const FittingInput &input, std::function<void(ParameterMap &&)> callback) { m_fitter.Fit(input, callback); }

}
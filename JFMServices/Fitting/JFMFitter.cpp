#include "JFMFitter.hpp"
#include "../Models/JFMAdditionalParameters.hpp"
#include "../Models/JFMModels.hpp"
#include "FittingSetup.hpp"

#include "../NumericStorm.hpp"
namespace JFMService
{

    template <size_t size>
    std::array<IVFittingSetup<size>, 3> AbstractFitter::transferFittingSetUp(const FittingInput &input)
    {
        std::array<IVFittingSetup<size>, 3> setUps;

        auto getPower = [](const ParameterMap &map)
        { return std::floor(std::log10(map[I0])); };

        double basePower = getPower(input);
        std::vector<double> I0s;
        utils::generateVectorAtGivenRanges(I0s, basePower - 1, basePower + 2, 1);
        ParameterMap initials = input.initialValues;
        std::array<double, size> min, max;

        for (int index : std::ranges::iota_view(0, size + 1))
        {
            min = initials.at((ParameterID)index) * 0.9;
            max = initials.at((ParameterID)index) * 1.1;
        }
        for (auto &setup : setUps)
        {
            setup.simplexMin{min};
            setup.simplexMax{max};
        }
        for (const auto &[destination, item] : std::views::zip(setUps, I0s))
        {
            destination.simplexMin[I0] = std::pow(10, item);
            destination.simplexMax[I0] = 9 * std::pow(10, item);
        }

        return setUps;
    }
    FourParameterFitter::FourParameterFitter()
    {
    }

    void FourParameterFitter::Fit(const FittingInput &input, Callback callback)
    {
        using NSFitter = NumericStorm::Fitting::Fitter<IVSimplexOptimizer<FourParameterModel>>;
        std::array<IVFittingSetup<4>, 3> setUps = transferFittingSetUp<4>(input);
        Data data = transferFittingData(input.initialData.characteristic);
        JFMAdditionalParameters additionalParameters;
        JFM for (auto &setUp : setUps)
            NSFitter fitter = getFitter<FourParameterModel, 4>(setUp);
    }
    void Fitter::Fit(const FittingInput &input, Callback callback) { fitterMap[input.initialData.modelID]->Fit(input, callback); }
    std::array<Data, 3> AbstractFitter::transferFittingData(const PlotData &input)
    {
        Data singleData(2);
        singleData[0] = std::vector<double>(input.voltageData.begin(), input.voltageData.end());
        singleData[1] = std::vector<double>(input.currentData.begin(), input.currentData.end());

        std::array<Data, 3> Data;
        return Data.fill(singleData);
    }
    std::array<JFMAdditionalParameters, 3> AbstractFitter::transferAdditionalParameters(const AdditionalParameterMap &input, const ParameterMap &fixingConfig)
    {
        JFMAdditionalParameters additional;
        additional.Temperature = input[Temperature];
        FixingConfiguration fixingConfiguration = 0;
        for (const auto &[key, value] : fixingConfig)
        {
            additional.fixingValues[key] = value;
            fixingConfiguration | BIT(key);
        }
        additional.fixingConfiguration = fixingConfiguration;
        std::array<JFMAdditionalParameters, 3> additionalParameters;

        return additionalParameters.fill(additional);
    }
}
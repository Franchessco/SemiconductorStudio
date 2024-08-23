#include "JFMFitter.hpp"
#include "../Models/JFMAdditionalParameters.hpp"
#include "../Models/JFMModels.hpp"
#include "../Models/CalculateData.hpp"
#include "FittingSetup.hpp"

#include "../NumericStorm.hpp"
namespace JFMService
{
	//! Abstract Fitter
	template <size_t size>
	IVFittingSetup<size> AbstractFitter::transferFittingSetUp(const FittingInput &input)
	{
		IVFittingSetup<size> setUp;

		ParameterMap initials = input.initialValues;
		std::array<double, size> min, max;

		for (int index : std::ranges::iota_view(0, size + 1))
		{
			min = initials.at((ParameterID)index) * 0.9;
			max = initials.at((ParameterID)index) * 1.1;
		}
		min[I0] = std::pow(10, item);
		max[I0] = 9 * std::pow(10, item);

		setup.simplexMin{min};
		setup.simplexMax{max};
		return setUp;
	}
	template <size_t size>
	NumericStorm::Fitting::Parameters<size> AbstractFitter::transferInitialPoint(const ParameterMap &initial)
	{
		NumericStorm::Fitting::Parameters<size> initialPoint;
		for (auto index : std::ranges::iota_view(0, size + 1))
			initialPoint[index] = initial.at((ParameterID)index);

		return initialPoint;
	}
	template <size_t size>
	SimplexOptimizationResults AbstractFitter::fit(const IVFittingSetup &setUp, const NumericStorm::Fitting::Parameters<size> &initialPoint, const Data &data, const AdditionalParameters &additionalParameters)
	{
		using NSFitter = NumericStorm::Fitting::Fitter<IVSimplexOptimizer<FourParameterModel>>;
		NSFitter fitter = getFitter<FourParameterModel, 4>(setUp);
		return fitter.fit(initialPoint, data, additionalParameters);
	}
	Data AbstractFitter::transferFittingData(const PlotData &input)
	{
		Data singleData(2);
		singleData[0] = std::vector<double>(input.voltageData.begin(), input.voltageData.end());
		singleData[1] = std::vector<double>(input.currentData.begin(), input.currentData.end());

		return singleData;
	}
	JFMAdditionalParameters AbstractFitter::transferAdditionalParameters(const AdditionalParameterMap &input, const ParameterMap &fixingConfig)
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

		return additional;
	}

	//! Four Parameter Fitter
	void FourParameterFitter::Fit(const FittingInput &input, Callback callback)
	{
		//! this can be rebuild and templated via model and number of parameters
		IVFittingSetup<4> setUps = transferFittingSetUp<4>(input);
		Data NSDdata = transferFittingData(input.initialData.characteristic);
		JFMAdditionalParameters additionalParameters = transferAdditionalParameters(input.initialData.additionalParameters, input.fixConfig);
		NumericStorm::Fitting::Parameters<4> initialPoint = transferInitialPoint(input.initialValues);

		SimplexOptimizationResults<4> results = fit<4>(setUp, initial, data, additional);
		ParameterMap fittingResult;
		for (const auto &[dst, key, src] : std::views::zip(results.getParameters(), fittingResult))
			dst = src;
		if (callback)
			callback(fittingResult);
	}

	//! Six Parameter Fitter
	void SixParameterFitter::Fit(const FittingInput &input, Callback callback)
	{
		//! this can be rebuild
		IVFittingSetup<6> setUps = transferFittingSetUp<6>(input);
		Data NSDdata = transferFittingData(input.initialData.characteristic);
		JFMAdditionalParameters additionalParameters = transferAdditionalParameters(input.initialData.additionalParameters, input.fixConfig);
		NumericStorm::Fitting::Parameters<6> initialPoint = transferInitialPoint(input.initialValues);

		SimplexOptimizationResults<6> results = fit<6>(setUp, initial, data, additional);
		ParameterMap fittingResult;
		for (const auto &[dst, key, src] : std::views::zip(results.getParameters(), fittingResult))
			dst = src;
		if (callback)
			callback(fittingResult);
	}

	//! General Fitter
	Fitter::Fitter()
	{
		fitterMap[Model4P] = std::make_shared<FourParameterFitter>();
		fitterMap[Model6P] = std::make_shared<SixParameterFitter>();
	}
	void Fitter::Fit(const FittingInput &input, Callback callback)
	{
		// todo add checking validity of item inside the map
		fitterMap[input.initialData.modelID]->Fit(input, callback);
	}

}
#include "JFMFitter.hpp"
#include "../Models/JFMAdditionalParameters.hpp"
#include "../Models/JFMModels.hpp"
#include "../Models/CalculateData.hpp"
#include "FittingSetup.hpp"

#include "../NumericStorm.hpp"
namespace JFMService::Fitters
{
	//! Abstract Fitter
	template <size_t size>
	IVFittingSetup<size> transferFittingSetUp(const FittingInput &input)
	{
		IVFittingSetup<size> setUp;

		ParameterMap initials = input.initialValues;
		std::array<double, size> min, max;

		for (int index : std::ranges::iota_view(0, (int)(size + 1)))
		{
			min[index] = initials[(ParameterID)index] * 0.9;
			max[index] = initials[(ParameterID)index] * 1.1;
		}
		double power = std::floor(std::log10(min[(ParameterID)I0]));
		min[I0] = std::pow(10, power);
		max[I0] = 9 * std::pow(10, power);

		setUp.simplexMin = min;
		setUp.simplexMax = max;
		return setUp;
	}
	template <size_t size>
	NumericStorm::Fitting::Parameters<size> transferInitialPoint(const ParameterMap &initial)
	{
		NumericStorm::Fitting::Parameters<size> initialPoint;
		for (auto index : std::ranges::iota_view(0, (int)(size + 1)))
			initialPoint[index] = initial.at((ParameterID)index);

		return initialPoint;
	}
	template <class Model, size_t size>
	SimplexOptimizationResults<size> fit(const IVFittingSetup<size> &setUp, const NumericStorm::Fitting::Parameters<size> &initialPoint, const Data &data, const JFMAdditionalParameters &additionalParameters)
	{
		using NSFitter = NumericStorm::Fitting::Fitter<IVSimplexOptimizer<Model>>;
		NSFitter fitter = getFitter<Model, size>(setUp);
		return fitter.fit(initialPoint, data, additionalParameters);
	}
	Data transferFittingData(const PlotData &input)
	{
		Data singleData(2);
		singleData[0] = std::vector<double>(input.voltageData.begin(), input.voltageData.end());
		singleData[1] = std::vector<double>(input.currentData.begin(), input.currentData.end());

		return singleData;
	}
	JFMAdditionalParameters transferAdditionalParameters(const AdditionalParameterMap &input, const ParameterMap &fixingConfig)
	{
		JFMAdditionalParameters additional;
		additional.Temperature = input.at(Temperature);
		FixingConfiguration fixingConfiguration = static_cast<FixingConfiguration>(0);
		JFMParameters destination;
		for (const auto &[dst, src] : std::views::zip(destination.getParameters(), fixingConfig))
		{
			auto &[key, val] = src;
			dst = val;
			fixingConfiguration | BIT(key);
		}
		additional.fixingConfiguration = fixingConfiguration;

		return additional;
	}

	//! Four Parameter Fitter
	void FourParameterFitter::Fit(const FittingInput &input, Callback callback)
	{
		//! this can be rebuild and templated via model and number of parameters
		IVFittingSetup<4> setUp = transferFittingSetUp<4>(input);
		Data NSDdata = transferFittingData(input.initialData.characteristic);
		JFMAdditionalParameters additionalParameters = transferAdditionalParameters(input.initialData.additionalParameters, input.fixConfig);
		NumericStorm::Fitting::Parameters<4> initialPoint = transferInitialPoint<4>(input.initialValues);

		SimplexOptimizationResults<4> results = fit<FourParameterModel, 4>(setUp, initialPoint, NSDdata, additionalParameters);
		ParameterMap fittingResult;

		for (const auto &[dst, src, index] : std::views::zip(fittingResult, results.getParameters(), std::ranges::iota_view(0, 5)))
			dst.second = src;

		if (callback)
			callback(std::move(fittingResult));
	}

	//! Six Parameter Fitter
	void SixParameterFitter::Fit(const FittingInput &input, Callback callback)
	{
		//! this can be rebuild
		IVFittingSetup<6> setUp = transferFittingSetUp<6>(input);
		Data NSDdata = transferFittingData(input.initialData.characteristic);
		JFMAdditionalParameters additionalParameters = transferAdditionalParameters(input.initialData.additionalParameters, input.fixConfig);
		NumericStorm::Fitting::Parameters<6> initialPoint = transferInitialPoint<6>(input.initialValues);

		SimplexOptimizationResults<6> results = fit<SixParameterModel, 6>(setUp, initialPoint, NSDdata, additionalParameters);
		ParameterMap fittingResult;

		for (const auto &[dst, src, index] : std::views::zip(fittingResult, results.getParameters(), std::ranges::iota_view(0, 7)))
			dst.second = src;

		if (callback)
			callback(std::move(fittingResult));
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

	std::shared_ptr<AbstractFitter> Fitter::operator[](ModelID id)
	{
		return fitterMap.at(id);
	}
}
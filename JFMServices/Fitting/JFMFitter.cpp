#include "JFMFitter.hpp"
#include "../Models/JFMAdditionalParameters.hpp"
#include "../Models/JFMModels.hpp"
#include "../Models/CalculateData.hpp"
#include "FittingSetup.hpp"

#include "../NumericStorm.hpp"
namespace JFMService::Fitters
{
	//! Abstract Fitter
	template <size_t parameter_size>
	IVFittingSetup<parameter_size> transferFittingSetUp(const FittingInput &input)
	{
		IVFittingSetup<parameter_size> setUp;

		ParameterMap initials = input.initialValues;
		std::array<double, parameter_size> min, max;

		for (int index : std::ranges::iota_view(0, (int)(parameter_size)))
		{
			min[index] = input.bounds.at((ParameterID)index).first;	 // * 0.9;
			max[index] = input.bounds.at((ParameterID)index).second; // *1.1;
		}
		// double power = std::floor(std::log10(min[(ParameterID)I0]));
		// min[I0] = std::pow(10, power);
		// max[I0] = 9 * std::pow(10, power);

		setUp.simplexMin = min;
		setUp.simplexMax = max;
		return setUp;
	}
	template <size_t parameter_size>
	NumericStorm::Fitting::Parameters<parameter_size> transferInitialPoint(const ParameterMap &initial)
	{
		NumericStorm::Fitting::Parameters<parameter_size> initialPoint;
		for (auto index : std::ranges::iota_view(0, (int)(parameter_size)))
			initialPoint[index] = initial.at((ParameterID)index);

		return initialPoint;
	}
	template <class Model, size_t parameter_size>
	SimplexOptimizationResults<parameter_size> fit(const IVFittingSetup<parameter_size> &setUp, const NumericStorm::Fitting::Parameters<parameter_size> &initialPoint, const Data &data, const JFMAdditionalParameters &additionalParameters)
	{
		using NSFitter = NumericStorm::Fitting::Fitter<IVSimplexOptimizer<Model>>;
		NSFitter fitter = getFitter<Model, parameter_size>(setUp);
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
		destination.getParameters().resize(6);
		for (const auto & src :  fixingConfig)
		{
			auto &[key, val] = src;
			destination[key] = val;
			fixingConfiguration |= (FixingConfiguration)BIT(key);
		}
		additional.fixingConfiguration = fixingConfiguration;
		additional.fixingValues = destination;
		return additional;
	}

	//! Four Parameter Fitter
	void FourParameterFitter::Fit(const FittingInput &input, Callback callback)
	{
		int fittingIterationRuns = 0;
		auto checkRepetitionCondition = [&](const SimplexOptimizationResults<4> &result)
		{
			auto parameters = result.getParameters();
			bool negativeValueParameters = std::ranges::any_of(parameters, [](double value)
															   { return value < 0; });
			bool bigError = result.getError() > 1 or result.getError()<0;
			bool iterationCondition = fittingIterationRuns < 5;
			//return false;
			return (negativeValueParameters or bigError) and iterationCondition;
		};
		//! this can be rebuild and templated via model and number of parameters

		IVFittingSetup<4> setUp = transferFittingSetUp<4>(input);
		Data NSDdata = transferFittingData(input.initialData.characteristic);
		JFMAdditionalParameters additionalParameters = transferAdditionalParameters(input.initialData.additionalParameters, input.fixConfig);
		NumericStorm::Fitting::Parameters<4> initialPoint = transferInitialPoint<4>(input.initialValues);
		auto recalculateBounds = [](IVFittingSetup<4>& setUp, NumericStorm::Fitting::Parameters<4>& initial) 
			{
				auto min = (NumericStorm::Fitting::SimplexPoint<4>(initial) * 0.1).getParameters().getParameters();
				auto max = (NumericStorm::Fitting::SimplexPoint<4>(initial) * 10).getParameters().getParameters();
				setUp.simplexMin = min;
				setUp.simplexMax = max;
			};
		auto recalculateInitialPoint = [](NumericStorm::Fitting::Parameters<4>& initial) 
			{
				for (auto& item : initial.getParameters())
					item *= Random::Float(0.1, 10);
			};
		SimplexOptimizationResults<4> results;
		auto transferFixingConfiguration = [&](const ParameterMap& additional)
			{
				auto& destination = results.getParameters();
				for (const auto& [key,val] : additional)
					if(additional.at(key))
						destination[(int)key] = val;
			};
		do
		{
			if (fittingIterationRuns > 1)
			{
				recalculateBounds(setUp, initialPoint);
				recalculateInitialPoint(initialPoint);
			}
			results = fit<FourParameterModel, 4>(setUp, initialPoint, NSDdata, additionalParameters);
			initialPoint = results.getParameters();
			if (additionalParameters.fixingConfiguration)
				transferFixingConfiguration(input.fixConfig);
			fittingIterationRuns += 1;
		} while (checkRepetitionCondition(results));

		ParameterMap fittingResult;

		for (const auto &[index,value] : std::views::enumerate(results.getParameters()))
			fittingResult[(Fitters::ParameterID)index] = value;

		if (callback)
			callback(std::move(fittingResult));
	}

	//! Six Parameter Fitter
	void SixParameterFitter::Fit(const FittingInput &input, Callback callback)
	{
		
		int fittingIterationRuns = 0;
		auto checkRepetitionCondition = [&](const SimplexOptimizationResults<6>& result)
			{
				auto parameters = result.getParameters();
				bool negativeValueParameters = std::ranges::any_of(parameters, [](double value)
					{ return value < 0; });
				bool bigError = result.getError() > 1 or result.getError() < 0;
				bool iterationCondition = fittingIterationRuns < 5;
				return false;
				return (negativeValueParameters or bigError) and iterationCondition;
			};
		//! this can be rebuild and templated via model and number of parameters

		IVFittingSetup<6> setUp = transferFittingSetUp<6>(input);
		Data NSDdata = transferFittingData(input.initialData.characteristic);
		JFMAdditionalParameters additionalParameters = transferAdditionalParameters(input.initialData.additionalParameters, input.fixConfig);
		NumericStorm::Fitting::Parameters<6> initialPoint = transferInitialPoint<6>(input.initialValues);
		auto recalculateBounds = [](IVFittingSetup<6>& setUp, NumericStorm::Fitting::Parameters<6>& initial)
			{
				auto min = (NumericStorm::Fitting::SimplexPoint<6>(initial) * 0.1).getParameters().getParameters();
				auto max = (NumericStorm::Fitting::SimplexPoint<6>(initial) * 10).getParameters().getParameters();
				setUp.simplexMin = min;
				setUp.simplexMax = max;
			};
		auto recalculateInitialPoint = [](NumericStorm::Fitting::Parameters<6>& initial)
			{
				for (auto& item : initial.getParameters())
					item *= Random::Float(0.1, 10);
			};
		SimplexOptimizationResults<6> results;
		auto transferFixingConfiguration = [&](const ParameterMap& additional)
			{
				auto& destination = results.getParameters();
				for (const auto& [key, val] : additional)
					if (additional.at(key))
						destination[(int)key] = val;
			};
		do
		{
			if (false)//fittingIterationRuns > 1)
			{
				recalculateBounds(setUp, initialPoint);
				recalculateInitialPoint(initialPoint);
			}
			results = fit<SixParameterModel, 6>(setUp, initialPoint, NSDdata, additionalParameters);
			//initialPoint = results.getParameters();
			if (additionalParameters.fixingConfiguration)
				transferFixingConfiguration(input.fixConfig);
			fittingIterationRuns += 1;
		} while (checkRepetitionCondition(results));

		ParameterMap fittingResult;

		for (const auto& [index, value] : std::views::enumerate(results.getParameters()))
			fittingResult[(Fitters::ParameterID)index] = value;

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
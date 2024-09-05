#include "MonteCarloEngine.hpp"
#include "../Models/JFMErrorModel.hpp"
#include "../Fitting/JFMFitter.hpp"
#include "../Models/CalculateData.hpp"
namespace JFMService
{
	MonteCarloEngine::MonteCarloEngine() {};
	void MonteCarloEngine::simulateChunk(int startIdx, int chunkSize, const std::shared_ptr<AbstractPreFit>& preFitter,
		const std::shared_ptr<Fitters::AbstractFitter> fitter, MCInput& input,
		std::vector<MCResult>& localResults,int numBlock)
	{
		//int blockNumber = startIdx - chunkSize
		for (int i = 0; i < chunkSize; ++i)
		{
			int idx = startIdx + i;
			if (idx >= input.iterations)
			{
				localResults.resize(i);
				break;
			}
			MCResult result;
			simulate(preFitter, fitter, input, localResults, i);
			
		std::cout << "block:" << numBlock<<" idx: "<<i << std::endl;
		}
	};
#define MULTI_THREAD  1;
	static std::mutex mutex;
	static int num = 0;
	void MonteCarloEngine::Simulate(const MCInput& input, std::function<void(MCOutput&&)> callback)
	{
		int chunkSize = (input.iterations / 24)+1; //41
		std::jthread thread{
			[&]()
			{
				MCOutput output;
				output.inputData = input;
				std::shared_ptr<Fitters::AbstractFitter> fitter = m_fitter[input.startingData.initialData.modelID];
				std::shared_ptr<AbstractPreFit> preFitter = m_prefitter[input.startingData.initialData.modelID];
				auto start = std::chrono::high_resolution_clock().now();
				output.mcResult.resize(input.iterations);

				std::vector<std::future<std::vector<MCResult>>> futures;
				int numChunks = (input.iterations + chunkSize - 1) / chunkSize; //25

				std::mutex resultMutex;
				std::vector<MCResult> finalResults(input.iterations);
				static int j = 0;
				for (int chunk = 0; chunk < numChunks; ++chunk) 
				{
					int startIdx = chunk * chunkSize;
					futures.push_back(std::async(std::launch::async,
						[&, startIdx]()
						{
							j+=1;
							std::vector<MCResult> localResults(chunkSize);
							simulateChunk(startIdx, chunkSize, preFitter, fitter, output.inputData, localResults,j--);
							return localResults; // Return local results
						}
					));
				}

				for (int chunk = 0; chunk < numChunks; ++chunk) {
					auto localResults = futures[chunk].get(); // Wait for and retrieve local results
					std::lock_guard<std::mutex> lock(resultMutex);
					std::copy(localResults.begin(), localResults.end(), output.mcResult.begin() + (chunk * chunkSize));
				}

				auto end = std::chrono::high_resolution_clock().now();
				std::cout << "time: " << (end - start).count() / 1000 << " s "
						  << (((end - start).count() / 1000) / input.iterations) << " ms per fit" << std::endl;

				if (callback)
					callback(std::move(output));
			}
		};
		
	}

	void MonteCarloEngine::generateNoise(double& value, double factor)
	{
		double noise= (value * factor / 100);
		double copy = value;
		//std::uniform_real_distribution<double> distribution{ -1,1 };
		std::normal_distribution<double> distribution{ 0,1};
		//std::cout << distribution(m_generator)*sigma << std::endl;
		value += distribution(m_generator) * noise;
		//value = value +  distribution(m_generator)*(factor / 100) * value ;
		//value = std::abs(value);
		
	}
	void MonteCarloEngine::simulate(const std::shared_ptr<AbstractPreFit>& preFitter,const std::shared_ptr<Fitters::AbstractFitter> fitter, MCInput& input,std::vector<MCResult>& results,int i)
	{
		MCResult result;
		MCInput copied{ input };
		auto characteristic = input.startingData.initialData.characteristic;
		std::vector<double> copiedCurrent{ characteristic.currentData.begin(),characteristic.currentData.end() };
		std::vector<double> copiedVoltage{ characteristic.voltageData.begin(),characteristic.voltageData.end() };
		copied.startingData.initialData.characteristic.currentData = { copiedCurrent.begin(),copiedCurrent.end() };

		auto callback = [&](const ParameterMap&& fittingResult)
			{ result.foundParameters = fittingResult; };

		for (auto& I : copiedCurrent)
			generateNoise(I, copied.noise);
		
		copied.startingData.initialValues = preFitter->Estimate(copied.startingData.initialData);
		fitter->Fit(copied.startingData, callback);
		
		calculateFittingError(input, result);
		results[i] = result;
/*		num += 1;
		std::cout << "iteration: " << num << std::endl;*/
	}
	void MonteCarloEngine::calculateFittingError(const MCInput& input, MCResult& result)
	{
		auto IerrorModel = [&](double trueI, double fittedI, double noise) 
			{
				double sigma = fittedI* noise;
				return std::pow(((fittedI - trueI) / sigma), 2);
			};
		CalculatingData data;
		DataCalculator dataCalculator;
		
		Chi2ErrorModel errorModel;
		auto characteristic = input.startingData.initialData.characteristic;
		std::vector<double> copiedCurrent{ characteristic.currentData.begin(),characteristic.currentData.end() };
		std::vector<double> copiedVoltage{ characteristic.voltageData.begin(),characteristic.voltageData.end() };

		data.additionalParameters = input.startingData.initialData.additionalParameters;
		data.parameters = result.foundParameters;
		data.characteristic = { {copiedVoltage.begin(),copiedVoltage.end()}, {copiedCurrent.begin(),copiedCurrent.end()} };
		data.modelID = input.startingData.initialData.modelID;

		
		std::vector<double> trueCurrentVector{};
		trueCurrentVector.resize(copiedCurrent.size());
		CalculatingData trueData = data;
		trueData.parameters = input.trueParameters;
		trueData.characteristic.currentData = std::span<double>{ trueCurrentVector };
		
		dataCalculator.CalculateData(data);
		dataCalculator.CalculateData(trueData);
		std::span<double> trueCurrent{trueCurrentVector};
		std::span<double> fittedCurrent = data.characteristic.currentData;
		double accumulatedError = 0.0;
		double noise = input.noise / 100.0;
		for (const auto& [trueI, fitI] : std::views::zip(trueCurrent, fittedCurrent))
			accumulatedError += IerrorModel(trueI, fitI, noise);

		result.error = accumulatedError;

	}
	double MonteCarloEngine::GetUncertainty(const MCOutput& output, int level, ParameterID id)
	{
		std::vector<double> values;
		values.reserve(output.mcResult.size());

		std::ranges::transform(output.mcResult, std::back_inserter(values), [&](const MCResult& result)
			{ return result.foundParameters.at(id); });

		if (values.size() < 2)
		{
			throw std::invalid_argument("Insufficient data to calculate uncertainty");
		}

		double max_value = *std::ranges::max_element(values);
		double min_value = *std::ranges::min_element(values);
		double difference = max_value - min_value;

		double multiplier = getUncertaintyMultiplier(
			calculateNumberOfFindingParameters(output.inputData.trueParameters,
				output.inputData.startingData.fixConfig),
			(ConfidenceLevel)level);

		return difference * multiplier;
	}

	int MonteCarloEngine::calculateNumberOfFindingParameters(const ParameterMap& trueParameters, const ParameterMap& fixedValues)
	{
		return trueParameters.size() - fixedValues.size();
	}

	MonteCarloEngine::UncertaintyMultipliers MonteCarloEngine::initializeUncertaintyMultipliers()
	{
		UncertaintyMultipliers multipliers;
		std::vector<double> values;
		utils::generateVectorAtGivenRanges(values, 1, 7, 1);
		auto correlateNumberOfParametersWithMultiplier = [&](const std::vector<double> mult)
			{
				std::unordered_map<uint8_t, double> multipliers;
				for (const auto& [parNumber, val] : std::views::zip(values, mult))
					multipliers[parNumber] = val;
				return multipliers;
			};
		std::vector<double> oneSigmaValues{ 1.00, 2.30, 3.53, 4.72, 5.89, 7.04 };
		std::vector<double> twoSigmaValues{ 4.00, 6.17, 8.02, 9.70, 11.3, 12.8 };
		std::vector<double> threeSigmaValues{ 9.00, 11.8, 14.2, 16.3, 18.2, 20.1 };
		multipliers[oneSigma] = correlateNumberOfParametersWithMultiplier(oneSigmaValues);
		// 90% 2.71 4.61 6.25 7.78 9.24 10.6
		multipliers[twoSigma] = correlateNumberOfParametersWithMultiplier(twoSigmaValues);
		// 99% 6.63 9.21 11.3 13.3 15.1 16.8
		multipliers[threeSigma] = correlateNumberOfParametersWithMultiplier(threeSigmaValues);
		// 99.99% 15.1 18.4 21.1 23.5 25.7 27.8
		return multipliers;
	}
	double MonteCarloEngine::getUncertaintyMultiplier(uint8_t numberOfParameters, ConfidenceLevel level)
	{
		return m_uncertaintyMultipliers.at(level).at(numberOfParameters);
	}
	double MonteCarloEngine::calculateMaximumError(const PlotData& trueData, double noiseFactor)
	{
		std::vector<double> min{ trueData.currentData.begin(), trueData.currentData.end() };
		std::vector<double> max{ trueData.currentData.begin(), trueData.currentData.end() };
		double minFactor{ 1 - noiseFactor }, maxFactor{ 1 + noiseFactor };
		std::ranges::for_each(min, [&](double& x)
			{ return x *= minFactor; });
		std::ranges::for_each(max, [&](double& x)
			{ return x *= maxFactor; });
		Chi2ErrorModel errorModel;
		double error = errorModel.CalculateError({ min.begin(), min.end() }, { max.begin(), max.end() });
		return error;
	}

}
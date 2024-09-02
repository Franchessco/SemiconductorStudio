#include "pch.hpp"
#include "Characteristic.hpp"

namespace JFMApp::Data
{
	Characteristic::Characteristic(const CharacteristicData& loaded)
	{
		
		V = loaded.Data[CharacteristicData::Voltage];
		I = loaded.Data[CharacteristicData::Current];
		T = loaded.Temperature;
		name = loaded.Name;
		modelID = 0;
		savedModelID = modelID;
		initState();
	}

	void Characteristic::initState() {
		mcMutex = std::make_shared<std::mutex>();
	}

	EstimateInput Characteristic::getEstimateInput() {
		EstimateInput input{};


		std::span<double> eV{ V.begin() + dataRange.first, V.begin() + dataRange.second };
		std::span<double> eI{ I.begin() + dataRange.first, I.begin() + dataRange.second };

		//FIX this in the future - abstract parameters in the loading service
		input.additionalParameters[6] = T;
		input.modelID = savedModelID;
		input.characteristic = { eV, eI };

		return input;
	}

	FittingInput Characteristic::getFittingInput() {
		FittingInput input{};

		input.initialData = getEstimateInput();
		input.name = name;
		input.fixConfig = savedFixedParametersValues;
		for (auto& [id, val] : savedFixedParameterIDs) {
			if (val)
				input.fixConfig.erase(id);
		}

		if(savedUseBounds)
			input.bounds = savedBounds;

		if(savedUseInitial)
			input.initialValues = savedInitialGuess;

		input.useBounds = savedUseBounds;

		return input;
	}

	void Characteristic::submitFitting(const ParameterMap& parameters, double error) {
		fittedParameters = parameters;
		tunedParameters = parameters;
		fixedParametersValues = parameters;
		savedFixedParametersValues = parameters;
		for (const auto& [k, v] : parameters) {
			fixedParameterIDs[k] = false;
			savedFixedParameterIDs[k] = false;
		}
		initialGuess = parameters;
		savedInitialGuess = parameters;
		tunedI = fittedI;
		isFitted = true;
		fitError = error;
	}

	CalculatingData Characteristic::getCalculatingData() {
		CalculatingData data{};
		data.characteristic = getEstimateInput().characteristic;
		fittedI.resize(data.characteristic.currentData.size());
		data.characteristic.currentData = std::span<double>{ fittedI };
		data.parameters = fittedParameters;
		data.additionalParameters = { {6, T} };
		data.modelID = modelID;
		return data;
	}

	CalculatingData Characteristic::getTuningData() {
		CalculatingData data{};
		data.characteristic = getEstimateInput().characteristic;
		tunedI.resize(data.characteristic.currentData.size());
		data.characteristic.currentData = std::span<double>{ tunedI };
		data.parameters = tunedParameters;
		data.additionalParameters = { {6, T} };
		data.modelID = modelID;
		return data;
	}

	MCInput Characteristic::getMCConfig() {
		MCInput input;

		input.startingData = getFittingInput();
		input.relPath = "./MC";
		input.trueParameters = fittedParameters;
		input.iterations = savedMCConfig.n;
		input.noise = savedMCConfig.sigma;

		return input;
	}


	void Characteristic::submitMC(const MCOutput& out) {
		MCSimulation sim{};
		sim.data.resize(out.mcResult.size());
		for (const auto& [src, dest] : std::views::zip(out.mcResult, sim.data)) {
			dest = src;
		}
		sim.sigma = out.inputData.noise;
		sim.fixConfig = out.inputData.startingData.fixConfig;

		sim.parent = this;
		std::scoped_lock lk{ *mcMutex };
		mcData.push_back(sim);
	}

}
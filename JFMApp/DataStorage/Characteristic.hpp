#pragma once

#include "pch.hpp"

#include "NumericsConfig.hpp"
#include "imgui.h"

#include "implot.h"

namespace JFMApp::Data {

	struct Characteristic
	{
		//containers of V, I, J
		//temperature
		//name
		//description
		//montecarlo sub characteristic
		//fitted characteristic
		//validity of subcharacteristics
		//data range

		//fitting configuration
		//--model
		//--fix
		//--initial guess
		//--mc
		//--notes

		std::vector<double> V{};
		std::vector<double> I{};
		std::vector<double> J{};

		double T{};

		std::pair<size_t, size_t> dataRange{};
		bool isFitted{ false };
		bool isSimulated{ false };
		bool checked{ false };

		ImVec4 color{ 0.96f, 0.53f, 0.26f, 1.0f };
		float weight{ 1.0 };

		std::string name{};
		std::string description{};

		//montecarlo
		struct MCData {
			ParameterMap parameters{};
			double error{ -1.0 };
			ParameterMap fixConfig{};
		};
		

		


		//fitted
		ParameterMap fittedParameters{};
		std::vector<double> fittedI{};

		double fitError{ -1.0 };

		//tuned
		std::vector<double> tunedI{};
		ParameterMap tunedParameters{};
		std::unordered_map<ParameterID, bool> tempParametersActive{};

		double tuneError{ -1.0 };





		//fitting configuration
		ModelID modelID{};
		std::unordered_map<ParameterID, bool> fixedParameterIDs{};
		ParameterMap initialGuess{};
		struct MCConfig {
			size_t n{};
			size_t saveN{};
			double sigma{};

			std::partial_ordering operator<=>(const MCConfig&) const = default;

		} mcConfig{};
		ParamBounds bounds{};

		bool useBounds{ false }, useInitial{ false };

		std::string notes{};


		//last saved config
		ModelID savedModelID{};
		std::unordered_map<ParameterID, bool> savedFixedParameterIDs{};
		ParameterMap savedInitialGuess{};
		MCConfig savedMCConfig{};
		ParamBounds savedBounds{};

		bool savedUseBounds{ false }, savedUseInitial{ false };

		std::string savedNotes{};


		static double TFL(double v, void*) {
			return std::log(std::max(v, std::numeric_limits<double>::epsilon()));
		}

		static double TFNL(double v, void*) {
			return std::exp(v);
		}
	};
};



#pragma once

#include "pch.hpp"

#include "NumericsConfig.hpp"
#include "imgui.h"

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

		ImVec4 color{};

		std::string name{};
		std::string description{};

		//montecarlo
		std::vector<std::pair<std::unordered_map<ParameterID, double>, double>> mcParameters{};

		//fitted
		std::vector<double> fittedParameters{};
		std::vector<double> fittedI{};

		//tuned
		std::vector<double> tunedI{};


		//fitting configuration
		ModelID modelID{};
		std::unordered_map<ParameterID, bool> fixedParameterIDs{};
		std::unordered_map<ParameterID, double> initialGuess{};
		struct MCConfig {
			size_t n{};
			size_t saveN{};
			double sigma{};
		} mcConfig{};

		std::string notes{};


		static double TFL(double v, void*) { 
			return std::log(std::max(v, std::numeric_limits<double>::epsilon())); 
		}

		static double TFNL(double v, void*) { 
			return std::exp(v); 
		}
	};
};



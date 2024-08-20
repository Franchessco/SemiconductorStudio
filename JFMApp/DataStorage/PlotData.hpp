#pragma once
#include "pch.hpp"

#include "Characteristic.hpp"


namespace JFMApp::Data
{
	struct PlotData
	{
		//references to the selected characteristics
		//plot options
		//--scale
		//--color
		// --ranged
		// --fitted
		// --main
		//pointer to active
		//temporary parameters
		//parameters history 
		//callbacks
		//hide/show non-active

		std::vector<Characteristic>* characteristics{ nullptr };

		bool logX{ false };
		bool logY{ false };

		bool plotRanged{ false };
		bool plotFitted{ false };

		ImVec4 colorRanged{}, colorFitted{};

		bool hideNonActive{ false };

		Characteristic* active{ nullptr };

		std::unordered_map<ParameterID, double> tempParameters{};
		std::unordered_map<ParameterID, bool> tempParametersActive{};

		struct PlotSettings {
			ImPlotFlags flags = ImPlotFlags_NoLegend;
			ImPlotAxisFlags xFlags = ImPlotAxisFlags_None;
			ImPlotAxisFlags yFlags = ImPlotAxisFlags_None;
		} plotSettings{};
	};	
}




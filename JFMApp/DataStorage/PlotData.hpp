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

		ImVec4 colorRanged{ 0.61f, 0.26f, 0.96f, 1.0f }, colorFitted{ 0.0f, 1.0f, 0.0f, 1.0f };

		bool hideNonActive{ false };

		Characteristic* active{ nullptr };
		NumericsConfig* paramConfig{ nullptr };

		struct MCPlotsData {
			std::vector<Characteristic::MCData> mc{};
			Data::Characteristic* characteristic{ nullptr };
			std::array<ImVec4, 3> sig{ ImVec4{ 0.0f, 1.0f, 0.0f, 1.0f }, ImVec4{ 1.0f, 1.0f, 0.0f, 1.0f }, ImVec4{ 1.0f, 0.0f, 0.0f, 1.0f } };
			std::pair<ParameterID, ParameterID> parameters{};
			std::string name{};
			int tab{ 0 };
		};

		std::vector<MCPlotsData> mcPlots{};

		std::string mcTempName{};
		std::pair<ParameterID, ParameterID> mcTempParams{};

		ImVector<unsigned int> mcTabs{};

		MCPlotsData* activeMC{ nullptr };

		

		bool configAll{ false };

		static struct PlotSettings {
			ImPlotFlags flags = ImPlotFlags_NoLegend;
			ImPlotAxisFlags xFlags = ImPlotAxisFlags_None;
			ImPlotAxisFlags yFlags = ImPlotAxisFlags_None;
		} plotSettings;
	};
}




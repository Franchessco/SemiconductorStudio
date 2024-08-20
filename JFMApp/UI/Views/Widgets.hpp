#pragma once
#include "pch.hpp"

#include "imgui.h"
#include "implot.h"

#include "ViewData.hpp"


namespace JFMApp::Views {

	struct Widgets {
		void Window(const std::function<void()>& widget, ImGuiWindowFlags flags = ImGuiWindowFlags_None);
		void PlottingArea(Data::PlotData* data);
		void BrowserArea(Data::BrowserData* data);
		void CharacteristicInspector(Data::PlotData* data);
		void MonteCarloInspector(Data::PlotData* data);

		void CharacteristicSelector(Data::BrowserData* data);
		void FileSelector(Data::BrowserData* data);
		void ViewMenu(Data::UIState* data);
		void CharacteristicSettings(Data::BrowserData* data);
		void GenerateData();
	};
}
#pragma once
#include "pch.hpp"

#include "PlotData.hpp"
#include "BrowserData.hpp"
#include "UIState.hpp"


namespace JFMApp::Data {
	struct ViewData {
		PlotData plotData{};
		BrowserData browserData{};
		UIState uiState{};
		NumericsConfig nConfig{};
	};
}
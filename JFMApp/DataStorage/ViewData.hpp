#pragma once
#include "pch.hpp"

#include "PlotData.hpp"
#include "BrowserData.hpp"
#include "UIState.hpp"


namespace JFMApp::Data {
	struct ViewData {
		PlotData m_plotData{};
		BrowserData m_browserData{};
		UIState m_uiState{};
	};
}
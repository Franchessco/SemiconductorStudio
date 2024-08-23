#pragma once
#include "pch.hpp"

#include "Characteristic.hpp"

namespace JFMApp::Data
{
	struct BrowserData
	{
		//characteristics(by value)

		//loader
		//path
		//characteristics
		//folders

		std::vector<Characteristic> m_characteristics{};
		Data::NumericsConfig* nConfig{ nullptr };


		ImVec4 startColor{}, endColor{};

		std::filesystem::path rootPath{};
		std::filesystem::path currentPath{};

		std::vector<bool> m_selection{};


		std::function<void()> m_loadCallback{};
		std::function<void()> m_loadAllCallback{};

		std::function<void()> m_updateColorsCallback{};
		std::function<void()> m_invertSelectionCallback{};
		std::function<void()> m_selectAllCallback{};
		std::function<void()> m_unselectAllCallback{};
		std::function<void()> m_removeSelectedCallback{};
		std::function<void()> m_removeUnselectedCallback{};

		
	};
}



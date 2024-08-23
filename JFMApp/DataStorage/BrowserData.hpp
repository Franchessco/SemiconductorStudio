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
	};
}



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

		//generation data
		ModelID m_genModelID{};
		std::array<float, 2> m_voltageGenRange{};
		std::array<float, 2> m_tempRange{};
		size_t m_tempN{1};
		float m_voltageGenStep{};
		bool m_byStepN{true};
		size_t m_nSteps{};
		float m_genT{ 100.0 };
		enum GenType { Linear = 0, Log, Exponential, PerDecade };
		std::array<std::string, 4> m_genTypes{ "Linear", "Log", "Exponential", "Per Decade" };


		struct GenData
		{
			double start{};
			double end{};
			double step{};
			size_t nSteps{};
			GenType type{};
			double singleValue{};
			bool singleShot{};
		};

		std::unordered_map<ParameterID, GenData> m_paramGenData{};

		std::function<void()> m_singleShot{};
		std::function<void()> m_generateCallback{};


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



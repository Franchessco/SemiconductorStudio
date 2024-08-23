#include "pch.hpp"

#include "Widgets.hpp"

namespace JFMApp::Views {

	static void drawCharacteristicConfig(Data::Characteristic& ch, Data::NumericsConfig* nConf) {
		if (!nConf) {
			ImGui::Text("No config");
			return;
		}

		ImVec2 tableSize = ImVec2(ImGui::GetContentRegionAvail().x, 0.0);
		ImGuiTableFlags tableFlags = ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersInner;

		if (ImGui::BeginTable(ch.name.c_str(), 3, tableFlags, tableSize)) {
			ImGui::TableSetupColumn("Fixed parameters");
			ImGui::TableSetupColumn("Initial Values");
			ImGui::TableSetupColumn("Model");

			ImGui::TableHeadersRow();

			ImGui::TableNextRow();

			//Fixed parameters
			{

				if (ch.fixedParameterIDs != ch.savedFixedParameterIDs) {
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0, 0.0, 0.0, 1.0));
				}

				int columns = std::min(2, (int)ch.fixedParameterIDs.size());

				if (ImGui::BeginTable("Fix Params", columns, tableFlags & ~ImGuiTableFlags_BordersInner)) {
					ImGui::TableNextRow();
					for (auto& [key, val] : ch.fixedParameterIDs) {
						ImGui::Checkbox(nConf->parameters[key].c_str(), &val);
						ImGui::TableNextColumn();
						ImGui::InputDouble(nConf->parameters[key].c_str(), &ch.fixedParametersValues[key], 0.0, 0.0, "%e");
						ImGui::TableNextColumn();
					}

					ImGui::EndTable();
				}

				if (ch.fixedParameterIDs != ch.savedFixedParameterIDs) {
					ImGui::PopStyleColor();
				}

			}

			ImGui::TableNextColumn();

			//Initial values
			{
				if (ch.initialGuess != ch.savedInitialGuess || ch.useInitial != ch.savedUseInitial) {
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0, 0.0, 0.0, 1.0));
				}

				ImGui::Checkbox("Use initial guess", &ch.useInitial);
				for (auto& [key, val] : ch.initialGuess) {
					if (ImGui::InputDouble(nConf->parameters[key].c_str(), &val, 0.0, 0.0, "%e"))
						val = std::clamp(val, nConf->paramBounds[key].first, nConf->paramBounds[key].second);
				}

				if (ch.fixedParameterIDs != ch.savedFixedParameterIDs) {
					ImGui::PopStyleColor();
				}
			}

			ImGui::TableNextColumn();

			//Model
			{
				if (ch.modelID != ch.savedModelID) {
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0, 0.0, 0.0, 1.0));
				}

				for (auto& [id, name] : nConf->models) {
					if (ImGui::Selectable(name.c_str(), id == ch.modelID)) {
						ch.modelID = id;
					}
				}

				if (ch.fixedParameterIDs != ch.savedFixedParameterIDs) {
					ImGui::PopStyleColor();
				}
			}

			ImGui::EndTable();
		}

		if (ImGui::BeginTable("MCConfig", 2, tableFlags, tableSize)) {
			ImGui::TableSetupColumn("MC Config");
			ImGui::TableSetupColumn("Bounds");

			ImGui::TableHeadersRow();

			ImGui::TableNextRow();

			//MC config
			{
				if (ch.mcConfig != ch.savedMCConfig) {
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0, 0.0, 0.0, 1.0));
				}

				int n = ch.mcConfig.n;
				int saveN = ch.mcConfig.saveN;
				double sigma = ch.mcConfig.sigma;

				if (ImGui::InputInt("N", &n))
					n = std::max(1, n);

				if (ImGui::InputInt("Save N", &saveN))
					saveN = std::max(1, saveN);

				if (ImGui::InputDouble("Sigma", &sigma, 0.0, 0.0, "%e"))
					sigma = sigma < 0.0 ? 1.0 : sigma;


				ch.mcConfig = { (size_t)n, (size_t)saveN, sigma };

				if (ch.mcConfig != ch.savedMCConfig) {
					ImGui::PopStyleColor();
				}
			}

			ImGui::TableNextColumn();

			//Bounds
			{
				if (ch.bounds != ch.savedBounds) {
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0, 0.0, 0.0, 1.0));
				}

				ImGui::Checkbox("Use bounds", &ch.useBounds);

				for (auto& [id, bounds] : ch.bounds) {
					double min = bounds.first;
					double max = bounds.second;


					if (ImGui::InputDouble(nConf->parameters[id].c_str(), &min, 0.0, 0.0, "%e"))
						min = std::clamp(min, nConf->paramBounds[id].first, max);

					ImGui::SameLine();
					if (ImGui::InputDouble(nConf->parameters[id].c_str(), &max, 0.0, 0.0, "%e"))
						max = std::clamp(max, min, nConf->paramBounds[id].second);

				}


				if (ch.bounds != ch.savedBounds) {
					ImGui::PopStyleColor();
				}
			}

			ImGui::EndTable();
		}

	}

	static void drawGlobalConfig(Data::PlotData& data) {
		if (!data.paramConfig) {
			ImGui::Text("No config");
			return;
		}

		ImVec2 tableSize = ImVec2(ImGui::GetContentRegionAvail().x, 0.0);
		ImGuiTableFlags tableFlags = ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersInner;

		if (ImGui::BeginTable("Global config", 3, tableFlags, tableSize)) {
			ImGui::TableSetupColumn("Fixed parameters");
			ImGui::TableSetupColumn("Initial Values");
			ImGui::TableSetupColumn("Model");

			ImGui::TableHeadersRow();

			ImGui::TableNextRow();

			//Fixed parameters
			{

				if (data.globalFixedParameterIDs != data.savedGlobalFixedParameterIDs) {
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0, 0.0, 0.0, 1.0));
				}

				int columns = std::min(3, (int)data.globalFixedParameterIDs.size());

				if (ImGui::BeginTable("Fix Params", columns, tableFlags & ~ImGuiTableFlags_BordersInner)) {
					ImGui::TableNextRow();
					for (auto& [key, val] : data.globalFixedParameterIDs) {
						ImGui::Checkbox(data.paramConfig->parameters[key].c_str(), &val);
						ImGui::TableNextColumn();
					}

					ImGui::EndTable();
				}

				if (data.globalFixedParameterIDs != data.savedGlobalFixedParameterIDs) {
					ImGui::PopStyleColor();
				}

			}

			ImGui::TableNextColumn();

			//Initial values
			{
				ImGui::BeginDisabled();

				bool u{ false };

				ImGui::Checkbox("Use initial guess", &u);
				for (auto& [key, val] : data.paramConfig->parameters) {
					double z = 0.0;
					ImGui::InputDouble(val.c_str(), &z, 0.0, 0.0, "%e");
				}

				ImGui::EndDisabled();
			}

			ImGui::TableNextColumn();

			//Model
			{
				if (data.globalModelID != data.savedGlobalModelID) {
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0, 0.0, 0.0, 1.0));
				}

				for (auto& [id, name] : data.paramConfig->models) {
					if (ImGui::Selectable(name.c_str(), id == data.globalModelID)) {
						data.globalModelID = id;
					}
				}

				if (data.globalModelID != data.savedGlobalModelID) {
					ImGui::PopStyleColor();
				}
			}

			ImGui::EndTable();
		}

		if (ImGui::BeginTable("MCConfig", 2, tableFlags, tableSize)) {
			ImGui::TableSetupColumn("MC Config");
			ImGui::TableSetupColumn("Bounds");

			ImGui::TableHeadersRow();

			ImGui::TableNextRow();

			//MC config
			{
				if (data.globalMCConfig != data.savedGlobalMCConfig) {
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0, 0.0, 0.0, 1.0));
				}

				int n = data.globalMCConfig.n;
				int saveN = data.globalMCConfig.saveN;
				double sigma = data.globalMCConfig.sigma;

				if (ImGui::InputInt("N", &n))
					n = std::max(1, n);

				if (ImGui::InputInt("Save N", &saveN))
					saveN = std::max(1, saveN);

				if (ImGui::InputDouble("Sigma", &sigma, 0.0, 0.0, "%e"))
					sigma = sigma < 0.0 ? 1.0 : sigma;


				data.globalMCConfig = { (size_t)n, (size_t)saveN, sigma };

				if (data.globalMCConfig != data.savedGlobalMCConfig) {
					ImGui::PopStyleColor();
				}
			}

			ImGui::TableNextColumn();

			//Bounds
			{
				ImGui::BeginDisabled();

				bool u{ false };
				ImGui::Checkbox("Use bounds", &u);

				for (auto& [id, name] : data.paramConfig->parameters) {

					double val{ 0.0 };

					ImGui::InputDouble(name.c_str(), &val, 0.0, 0.0, "%e");

					ImGui::SameLine();
					ImGui::InputDouble(name.c_str(), &val, 0.0, 0.0, "%e");
				}


				ImGui::EndDisabled();
			}

			ImGui::EndTable();
		}

	}


	void Widgets::CharacteristicInspector(Data::PlotData& data) {
		if (!data.characteristics) {
			ImGui::Text("No characteristics");
			return;
		}

		ImGui::Checkbox("Select All", &data.configAll);

		ImVec2 inspectorSize = ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y * 0.9f);

		if (data.configAll) {
			drawGlobalConfig(data);
		}
		else if (ImGui::BeginChild("CharacteristicInspector", inspectorSize)) {


			for (auto& ch : *(data.characteristics)) {
				ImGui::CollapsingHeader(ch.name.c_str());
				drawCharacteristicConfig(ch, data.paramConfig);
			}

			ImGui::EndChild();
		}




		if (ImGui::Button("Update")) {
			if (data.configAll) {
				data.savedGlobalFixedParameterIDs = data.globalFixedParameterIDs;
				data.savedGlobalModelID = data.globalModelID;
				data.savedGlobalMCConfig = data.globalMCConfig;
			}
			else {
				for (auto& ch : *(data.characteristics)) {
					ch.savedBounds = ch.bounds;
					ch.savedFixedParameterIDs = ch.fixedParameterIDs;
					ch.savedFixedParametersValues = ch.fixedParametersValues;
					ch.savedInitialGuess = ch.initialGuess;
					ch.savedMCConfig = ch.mcConfig;
					ch.savedModelID = ch.modelID;
					ch.savedUseBounds = ch.useBounds;
					ch.savedUseInitial = ch.useInitial;
				}
			}
		}

		ImGui::SameLine();

		if (ImGui::Button("Restore")) {
			if (data.configAll) {
				data.globalFixedParameterIDs = data.savedGlobalFixedParameterIDs;
				data.globalModelID = data.savedGlobalModelID;
				data.globalMCConfig = data.savedGlobalMCConfig;
			}
			else {
				for (auto& ch : *(data.characteristics)) {
					ch.bounds = ch.savedBounds;
					ch.fixedParameterIDs = ch.savedFixedParameterIDs;
					ch.fixedParametersValues = ch.savedFixedParametersValues;
					ch.initialGuess = ch.savedInitialGuess;
					ch.mcConfig = ch.savedMCConfig;
					ch.modelID = ch.savedModelID;
					ch.useBounds = ch.savedUseBounds;
					ch.useInitial = ch.savedUseInitial;
				}
			}
		}

		ImGui::SameLine();

		if (ImGui::Button("Save MC config")) {
			data.m_saveMCConfCallback();
		}

		if (!data.configAll && ImGui::Button("Perform MC on active")) {
			data.m_performMCCallback();
		} else if(data.configAll && ImGui::Button("Perform MC on all")) {
			data.m_performMCOnAllCallback();
		}

	}
};
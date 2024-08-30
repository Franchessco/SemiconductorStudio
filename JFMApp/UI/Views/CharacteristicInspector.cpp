#include "pch.hpp"

#include "Widgets.hpp"

namespace JFMApp::Views {

	static void drawCharacteristicConfig(Data::Characteristic& ch, Data::NumericsConfig* nConf) {
		if (!nConf) {
			ImGui::Text("No config");
			return;
		}

		ImVec2 tableSize = ImVec2(ImGui::GetContentRegionAvail().x, 0.0);
		ImGuiTableFlags tableFlags = ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersInner | ImGuiTableFlags_SizingStretchProp;

		if (ImGui::BeginTable(ch.name.c_str(), 3, tableFlags, tableSize)) {
			ImGui::TableSetupColumn("Fixed parameters");
			ImGui::TableSetupColumn("Initial Values");
			ImGui::TableSetupColumn("Model");

			ImGui::TableHeadersRow();

			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			//Fixed parameters
			{
				bool col = false;
				if (ch.fixedParameterIDs != ch.savedFixedParameterIDs || ch.fixedParametersValues != ch.savedFixedParametersValues) {
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0, 0.0, 0.0, 1.0));
					col = true;
				}


				auto& params = nConf->modelParameters[ch.modelID];
				for (const auto& key : params) {
					std::string cname = "##" + nConf->parameters[key];
					ImGui::Checkbox(cname.c_str(), &ch.fixedParameterIDs[key]);
					ImGui::SameLine();
					if (ImGui::InputDouble(nConf->parameters[key].c_str(), &ch.fixedParametersValues[key], 0.0, 0.0, "%e") && ImGui::IsItemDeactivatedAfterEdit()) {
						ch.fixedParametersValues[key] = std::clamp(ch.fixedParametersValues[key], nConf->paramBounds[key].first, nConf->paramBounds[key].second);
					}
				}

				if (col) {
					ImGui::PopStyleColor();
				}

			}

			ImGui::TableNextColumn();

			//Initial values
			{
				bool col = false;
				if (ch.initialGuess != ch.savedInitialGuess || ch.useInitial != ch.savedUseInitial) {
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0, 0.0, 0.0, 1.0));
					col = true;
				}

				ImGui::Checkbox("Use initial guess", &ch.useInitial);
				auto& params = nConf->modelParameters[ch.modelID];
				for (auto& key : params) {
					std::string gname = nConf->parameters[key] + " guess";
					if (ImGui::InputDouble(gname.c_str(), &ch.initialGuess[key], 0.0, 0.0, "%e") && ImGui::IsItemDeactivatedAfterEdit())
						ch.initialGuess[key] = std::clamp(ch.initialGuess[key], nConf->paramBounds[key].first, nConf->paramBounds[key].second);
				}

				if (col) {
					ImGui::PopStyleColor();
				}
			}

			ImGui::TableNextColumn();

			//Model
			{
				bool col = false;
				if (ch.modelID != ch.savedModelID) {
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0, 0.0, 0.0, 1.0));
					col = true;
				}
				if (ImGui::BeginCombo("##Model", nConf->models[ch.modelID].c_str(), ImGuiComboFlags_WidthFitPreview)) {
					for (auto& [id, name] : nConf->models) {
						if (ImGui::Selectable(name.c_str(), id == ch.modelID)) {

							ch.modelID = id;
						}
					}

					ImGui::EndCombo();
				}


				if (col) {
					ImGui::PopStyleColor();
				}
			}

			ImGui::EndTable();
		}

		if (ImGui::BeginTable("MCConfig", 2, tableFlags, tableSize)) {
			ImGui::TableSetupColumn("MC Config", ImGuiTableColumnFlags_WidthFixed, tableSize.x * 0.25f);
			ImGui::TableSetupColumn("Bounds");

			ImGui::TableHeadersRow();

			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			//MC config
			{
				bool col = false;
				if (ch.mcConfig != ch.savedMCConfig) {
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0, 0.0, 0.0, 1.0));
					col = true;
				}

				int n = ch.mcConfig.n;
				int saveN = ch.mcConfig.saveN;
				double sigma = ch.mcConfig.sigma;

				if (ImGui::InputInt("N", &n))
					n = std::max(0, n);

				if (ImGui::InputInt("Save N", &saveN))
					saveN = std::max(0, saveN);

				if (ImGui::InputDouble("Sigma", &sigma, 0.0, 0.0, "%e") && ImGui::IsItemDeactivatedAfterEdit())
					sigma = sigma < 0.0 ? 1.0 : sigma;


				ch.mcConfig = { (size_t)n, (size_t)saveN, sigma };

				if (col) {
					ImGui::PopStyleColor();
				}
			}

			ImGui::TableNextColumn();

			//Bounds
			{
				bool col = false;
				if (ch.bounds != ch.savedBounds) {
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0, 0.0, 0.0, 1.0));
					col = true;
				}

				ImGui::Checkbox("Use bounds", &ch.useBounds);
				if (ImGui::BeginTable("Bounds", 3, tableFlags & ~ImGuiTableFlags_BordersInner)) {
					ImGui::TableNextRow();
					auto& params = nConf->modelParameters[ch.modelID];
					auto& bounds = nConf->paramBounds;
					for (auto& id : params) {
						double min = bounds[id].first;
						double max = bounds[id].second;

						std::string minName = "##min" + nConf->parameters[id];

						ImGui::TableNextColumn();
						ImGui::Text(nConf->parameters[id].c_str());
						ImGui::TableNextColumn();

						ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
						if (ImGui::InputDouble(minName.c_str(), &min, 0.0, 0.0, "%e"))
							min = std::clamp(min, nConf->paramBounds[id].first, max);
						
						ImGui::PopItemWidth();

						
						std::string maxName = "##max" + nConf->parameters[id];

						ImGui::TableNextColumn();
						ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
						if (ImGui::InputDouble(maxName.c_str(), &max, 0.0, 0.0, "%e"))
							max = std::clamp(max, min, nConf->paramBounds[id].second);

						ImGui::PopItemWidth();

					}

					ImGui::EndTable();
				}
				


				if (col) {
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
		ImGuiTableFlags tableFlags = ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersInner | ImGuiTableFlags_SizingStretchProp;

		if (ImGui::BeginTable("Global config", 2, tableFlags, tableSize)) {
			ImGui::TableSetupColumn("Fixed parameters");
			ImGui::TableSetupColumn("Model");

			ImGui::TableHeadersRow();

			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			//Fixed parameters
			{

				bool col = false;
				if (data.globalFixedParameterIDs != data.savedGlobalFixedParameterIDs) {
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0, 0.0, 0.0, 1.0));
					col = true;
				}


				int columns = std::max(3, (int)data.globalFixedParameterIDs.size());

				if (ImGui::BeginTable("Fix Params", 1, tableFlags & ~ImGuiTableFlags_BordersInner)) {
					ImGui::TableNextRow();
					auto& params = data.paramConfig->modelParameters[data.globalModelID];
					for (auto& key : params) {
						ImGui::TableNextColumn();
						ImGui::Checkbox(data.paramConfig->parameters[key].c_str(), &data.globalFixedParameterIDs[key]);
					}

					ImGui::EndTable();
				}

				if (col) {
					ImGui::PopStyleColor();
				}

			}


			ImGui::TableNextColumn();

			//Model
			{
				bool col = false;
				if (data.globalModelID != data.savedGlobalModelID) {
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0, 0.0, 0.0, 1.0));
					col = true;
				}
				if (ImGui::BeginCombo("##Model", data.paramConfig->models[data.globalModelID].c_str(), ImGuiComboFlags_WidthFitPreview)) {
					for (auto& [id, name] : data.paramConfig->models) {
						if (ImGui::Selectable(name.c_str(), id == data.globalModelID)) {
							data.globalModelID = id;
						}
					}

					ImGui::EndCombo();
				}
				

				if (col) {
					ImGui::PopStyleColor();
				}
			}

			ImGui::EndTable();
		}

		if (ImGui::BeginTable("MCConfig", 1, tableFlags, tableSize)) {
			ImGui::TableSetupColumn("MC Config");

			ImGui::TableHeadersRow();

			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			//MC config
			{
				bool col = false;
				if (data.globalMCConfig != data.savedGlobalMCConfig) {
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0, 0.0, 0.0, 1.0));
					col = true;
				}

				int n = data.globalMCConfig.n;
				int saveN = data.globalMCConfig.saveN;
				double sigma = data.globalMCConfig.sigma;

				if (ImGui::InputInt("N", &n))
					n = std::max(0, n);

				if (ImGui::InputInt("Save N", &saveN))
					saveN = std::max(0, saveN);

				if (ImGui::InputDouble("Sigma", &sigma, 0.0, 0.0, "%e") && ImGui::IsItemDeactivatedAfterEdit())
					sigma = sigma < 0.0 ? 1.0 : sigma;


				data.globalMCConfig = { (size_t)n, (size_t)saveN, sigma };

				if (col) {
					ImGui::PopStyleColor();
				}
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
		ImGuiChildFlags cFlags = ImGuiChildFlags_Border | ImGuiChildFlags_AutoResizeY;
		ImGui::BeginChild("CharacteristicInspector", inspectorSize, cFlags);

		if (data.configAll) {
			drawGlobalConfig(data);
		}
		else {


			for (auto& ch : *(data.characteristics)) {
				if (ch.checked && ImGui::CollapsingHeader(ch.name.c_str()))
					drawCharacteristicConfig(ch, data.paramConfig);
			}


		}

		{
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
			}
			else if (data.configAll && ImGui::Button("Perform MC on all")) {
				data.m_performMCOnAllCallback();
			}
		}

		ImGui::EndChild();





	}
};
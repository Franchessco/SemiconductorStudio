#include "pch.hpp"

#include "Widgets.hpp"

namespace JFMApp::Views {

	void Widgets::CharacteristicList(Data::BrowserData& data) {

		ImGui::BeginGroup();

		//control buttons
		{
			ImGui::ColorPicker4("Start Color", &data.startColor.x, ImGuiColorEditFlags_NoInputs);
			ImGui::SameLine();
			ImGui::ColorPicker4("End Color", &data.endColor.x, ImGuiColorEditFlags_NoInputs);

			ImGui::SameLine(0.0f, 20.0f);

			ImGui::BeginGroup();

			if (ImGui::Button("Update colors")) {

			}

			if (ImGui::Button("Invert Selection")) {

			}

			ImGui::EndGroup();

			ImGui::SameLine(0.0f, 20.0f);

			ImGui::BeginGroup();

			if (ImGui::Button("Select all")) {

			}

			if (ImGui::Button("Unselect all")) {

			}

			ImGui::EndGroup();

			ImGui::SameLine(0.0f, 20.0f);

			ImGui::BeginGroup();

			if (ImGui::Button("Remove selected")) {

			}

			if (ImGui::Button("Remove unselected")) {

			}

			ImGui::EndGroup();
		}


		ImGui::EndGroup();

		//characteristics list
		{
			auto* nConf = data.nConfig;

			int param_size = 4 + nConf ? nConf->parameters.size() : 0;

			ImGuiTableFlags flags = ImGuiTableFlags_BordersOuter | ImGuiTableFlags_Resizable;
			ImVec2 size = ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y * 0.5f);

			if (ImGui::BeginTable("Characteristics", param_size, flags, size)) {

				ImGui::TabItemButton("Color");
				ImGui::TableSetupColumn("Name");
				ImGui::TableSetupColumn("Error");
				ImGui::TableSetupColumn("T");

				if (param_size > 4)
					for (const auto& [key, val] : nConf->parameters)
						ImGui::TableSetupColumn(val.c_str());

				ImGui::TableHeadersRow();


				int id = 0;
				for (auto& ch : data.m_characteristics) {
					ImGui::TableNextRow();

					ImGui::PushID(id++);
					ImGui::ColorEdit4("", &ch.color.x, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
					ImGui::PopID();


					ImGui::TableNextColumn();
					if (ImGui::IsItemHovered())
						ch.weight = 2.0f;
					else
						ch.weight = 1.0f;

					ImGui::TableNextColumn();
					ImGui::Checkbox(ch.name.c_str(), &ch.checked);


					ImGui::TableNextColumn();
					ImGui::Text(ch.fitError > -1.0 ? std::to_string(ch.fitError).c_str() : "-");

					ImGui::TableNextColumn();
					ImGui::Text(std::to_string(ch.T).c_str());

					if (param_size > 4)
						for (const auto& [key, val] : nConf->parameters) {
							ImGui::TableNextColumn();

							std::string value = ch.fittedParameters.contains(key) ? std::to_string(ch.fittedParameters[key]) : "-";
							ImGui::Text(value.c_str());
						}
				}

				ImGui::EndTable();
			}
		}

	}

};
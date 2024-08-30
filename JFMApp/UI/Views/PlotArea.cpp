#include "pch.hpp"

#include "Widgets.hpp"

namespace JFMApp::Views {
	void Widgets::PlottingArea(Data::PlotData& data) {
		if (!data.characteristics || !data.paramConfig) return;

		//plot and list are in the table
		ImVec2 tableSize = ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y * 0.65f);
		ImGuiTableFlags tableFlags = ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_SizingStretchProp;

		if (ImGui::BeginTable("PlottingArea", 2, tableFlags, tableSize)) {
			if (tableSize.x > 10) {
				ImGui::TableSetupColumn("Plot", ImGuiTableColumnFlags_WidthFixed, tableSize.x * 0.8f);
				ImGui::TableSetupColumn("List", ImGuiTableColumnFlags_WidthStretch);
			}
			ImGui::TableHeadersRow();

			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			//draw the plot area
			{


				ImVec2 plotAreaSize = ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y * 0.75f);
				if (ImPlot::BeginPlot("Characteristics", plotAreaSize, data.plotSettings.flags)) {
					ImPlot::SetupAxes("V", "I", Data::PlotData::plotSettings.xFlags, Data::PlotData::plotSettings.yFlags);

					Data::PlotData::plotSettings.xFlags &= ~ImPlotAxisFlags_AutoFit;
					Data::PlotData::plotSettings.yFlags &= ~ImPlotAxisFlags_AutoFit;

					if (data.logX)
						ImPlot::SetupAxisScale(ImAxis_X1, Data::Characteristic::TFL, Data::Characteristic::TFNL);

					if (data.logY)
						ImPlot::SetupAxisScale(ImAxis_Y1, Data::Characteristic::TFL, Data::Characteristic::TFNL);



					if (data.active && data.hideNonActive) {
						auto& act = *(data.active);
						//main characteristics
						ImPlot::SetNextLineStyle(act.color, act.weight);
						ImPlot::PlotLine("Active", act.V.data(), act.I.data(), std::min(act.V.size(), act.I.size()));


						//ranged characteristics
						if (data.plotRanged) {
							ImPlot::SetNextLineStyle(data.colorRanged);
							ImPlot::PlotLine(act.name.c_str(), act.V.data() + act.dataRange.first, act.I.data() + act.dataRange.first, act.dataRange.second - act.dataRange.first + 1);
						}

						//fitted characteristics
						if (act.isFitted && data.plotFitted) {
							ImPlot::SetNextLineStyle(data.colorFitted);
							ImPlot::PlotLine(act.name.c_str(), act.V.data() + act.dataRange.first, act.fittedI.data(), act.fittedI.size());
						}

						//tuned characteristics
						if (act.isFitted && data.plotFitted) {
							ImPlot::SetNextLineStyle(data.colorFitted, 2.0);
							ImPlot::PlotLine(act.name.c_str(), act.V.data() + act.dataRange.first, act.tunedI.data(), act.tunedI.size());
						}

					}
					else if (!data.hideNonActive && data.characteristics) {
						for (auto& ch : *(data.characteristics)) {
							if(!ch.checked) continue;

							//main characteristics
							ImPlot::SetNextLineStyle(ch.color, ch.weight);
							ImPlot::PlotLine(ch.name.c_str(), ch.V.data(), ch.I.data(), std::min(ch.V.size(), ch.I.size()));

							//ranged characteristics
							if (data.plotRanged) {
								ImPlot::SetNextLineStyle(data.colorRanged);
								ImPlot::PlotLine(ch.name.c_str(), ch.V.data() + ch.dataRange.first, ch.I.data() + ch.dataRange.first, ch.dataRange.second - ch.dataRange.first + 1);
							}

							//fitted characteristics
							if (ch.isFitted && data.plotFitted) {
								ImPlot::SetNextLineStyle(data.colorFitted);
								ImPlot::PlotLine(ch.name.c_str(), ch.V.data() + ch.dataRange.first, ch.fittedI.data(), ch.fittedI.size());
							}

							//tuned characteristics
							if (ch.isFitted && data.plotFitted) {
								ImPlot::SetNextLineStyle(data.colorFitted, 2.0);
								ImPlot::PlotLine(ch.name.c_str(), ch.V.data() + ch.dataRange.first, ch.tunedI.data(), ch.tunedI.size());
							}

						}
					}

					//plot infinite lines

					if(data.active && data.plotRanged){
						auto& act = *(data.active);
						if(act.dataRange.first != act.dataRange.second){
							ImPlot::SetNextLineStyle(data.colorRanged);
							ImPlot::PlotInfLines("Down", &act.V[act.dataRange.first], 1);
							ImPlot::SetNextLineStyle(data.colorRanged);
							ImPlot::PlotInfLines("Up", &act.V[act.dataRange.second], 1);
						}
					}

					ImPlot::EndPlot();
				}
			}

			ImGui::TableNextColumn();
			//draw the characteristic list
			{
				ImVec2 listSize = ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y * 0.75f);
				if (ImGui::BeginListBox("##List", listSize)) {
					if (data.characteristics) {

						for (auto& ch : *(data.characteristics)) {
							if (!ch.checked) continue;
							if (ImGui::Selectable(ch.name.c_str(), data.active == &ch)) {
								data.active = &ch;
							}
						}

					}
					ImGui::EndListBox();
				}
			}

			ImGui::EndTable();
		}

		ImGui::BeginChild("CharacteristicSettings");
		ImGui::BeginGroup();
		{
			ImGui::Separator();

			//plot settings
			{
				if (ImGui::Button("Fit plot area")) {
					Data::PlotData::plotSettings.xFlags |= ImPlotAxisFlags_AutoFit;
					Data::PlotData::plotSettings.yFlags |= ImPlotAxisFlags_AutoFit;
				}

				ImGui::SameLine(0.0, 20.0);

				ImGui::Checkbox("Log X", &data.logX);
				ImGui::SameLine();
				ImGui::Checkbox("Log Y", &data.logY);

				ImGui::SameLine(0.0, 20.0);
				ImGui::Checkbox("Plot Ranged", &data.plotRanged);
				ImGui::SameLine();
				ImGui::ColorEdit4("Ranged Color", &data.colorRanged.x, ImGuiColorEditFlags_NoInputs);

				ImGui::SameLine(0.0, 20.0);
				ImGui::Checkbox("Plot Fitted", &data.plotFitted);
				ImGui::SameLine();
				ImGui::ColorEdit4("Fitted Color", &data.colorFitted.x, ImGuiColorEditFlags_NoInputs);

				ImGui::Checkbox("Hide non-active", &data.hideNonActive);
			}

			ImGui::Separator();


			if (!data.active) {
				ImGui::BeginDisabled();
			}

			//in case data->active is a nullptr
			static Data::Characteristic chr{};
			auto& act = data.active ? *(data.active) : chr;

			float groupH{ 0 };

			ImVec2 child_size = ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y);

			ImGui::BeginChild("CharSetChild", child_size, ImGuiChildFlags_Border | ImGuiChildFlags_AutoResizeY);

			//draw the characteristic settings
			{

				int lower = 0;
				int middle = (int)act.dataRange.first + 1;
				int upper = act.V.size() - 1;
				groupH = ImGui::GetCursorPosY();
				ImGui::BeginGroup();

				auto vStr = data.active ? std::to_string(act.V[act.dataRange.second]) : "0";

				float sliderW = ImGui::GetCursorPosX();

				if (ImGui::SliderInt("Up range", (int*)&(act.dataRange.second), middle, upper, vStr.c_str())) {

				}

				sliderW = ImGui::GetCursorPosX() - sliderW;

				ImGui::SameLine(0.0, 50.0);

				float buttonW = ImGui::GetCursorPosX();

				if (ImGui::Button("Estimate")) {
					data.m_estimateCallback();
				}

				buttonW = ImGui::GetCursorPosX() - buttonW;

				vStr = data.active ? std::to_string(act.V[act.dataRange.first]) : "0";

				ImGui::PushItemWidth(sliderW);

				middle = std::max(lower, (int)act.dataRange.second - 1);

				if (ImGui::SliderInt("Down range", (int*)&(act.dataRange.first), lower, middle, vStr.c_str())) {

				}

				ImGui::PopItemWidth();

				ImGui::SameLine(0.0, 50.0);
				ImGui::PushItemWidth(buttonW);

				if (ImGui::Button("Fit")) {
					data.m_fitCallback();
				}

				ImGui::PopItemWidth();

				
				groupH = ImGui::GetCursorPosY() - groupH;

				ImGui::Text("Tune Error: %e", act.tuneError >= 0.0 ? act.tuneError : 0.0);

				ImGui::SameLine(0.0f, 20.0f);

				if (ImGui::Button("Update characteristic")) {
					for (auto& [key, val] : act.fittedParameters) {
						if (act.tempParametersActive[key]) val = act.tunedParameters[key];
					}

					act.m_tuneCallback();
				}

				ImGui::EndGroup();
			}

			ImGui::SameLine(0.0, 30.0);

			//draw the parameter tables
			{
				ImGuiTableFlags tableFlags = ImGuiTableFlags_Borders
					| ImGuiTableFlags_Resizable
					| ImGuiTableFlags_Reorderable
					| ImGuiTableFlags_Hideable
					|ImGuiTableFlags_SizingStretchSame;

				ImVec2 tableSize = ImVec2( ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y );

				if (ImGui::BeginTable("Parameters", 2, tableFlags, tableSize)) {

					ImGui::TableSetupColumn("Fitted");
					ImGui::TableSetupColumn("Tuned");

					ImGui::TableHeadersRow();

					ImGui::TableNextRow();
					ImGui::TableNextColumn();
					if (ImGui::BeginTable("Fitted", 2, tableFlags & ~ImGuiTableFlags_Reorderable)) {
						ImGui::TableSetupColumn("Parameter");
						ImGui::TableSetupColumn("Value");
						ImGui::TableHeadersRow();

						for (auto& [id, value] : act.fittedParameters) {
							ImGui::TableNextRow();
							ImGui::TableNextColumn();
							ImGui::Text(data.paramConfig->parameters[id].c_str());
							ImGui::TableNextColumn();
							ImGui::Text(std::to_string(value).c_str());
						}

						ImGui::EndTable();
					}

					ImGui::TableNextColumn();

					if (ImGui::BeginTable("Tuned", 2, tableFlags & ~ImGuiTableFlags_Reorderable)) {
						ImGui::TableSetupColumn("Parameter");
						ImGui::TableSetupColumn("Value");
						ImGui::TableHeadersRow();

						for (auto& [id, value] : act.tunedParameters) {
							ImGui::TableNextRow();
							ImGui::TableNextColumn();
							ImGui::Text(data.paramConfig->parameters[id].c_str());
							ImGui::TableNextColumn();
							ImGui::Text(std::to_string(value).c_str());
						}

						ImGui::EndTable();
					}

					ImGui::EndTable();
				}

			}

			ImGui::Separator();

			//draw the parameter sliders
			{
				if (ImGui::BeginTable("Tuned parameters", 3)) {
					ImGui::TableNextRow();
					for (auto& [id, value] : act.tunedParameters) {
						float val = value;
						float min = data.paramConfig->paramBounds[id].first;
						float max = data.paramConfig->paramBounds[id].second;

						ImGui::TableNextColumn();
						std::string cname = "##" + data.paramConfig->parameters[id];
						if (ImGui::Checkbox(cname.c_str(), &act.tempParametersActive[id]))
							if (act.tempParametersActive[id]) data.m_tuneCallback();

						ImGui::SameLine();
						
						if (ImGui::InputFloat(data.paramConfig->parameters[id].c_str(), &val, 0.0f, 0.0f, "%e") && ImGui::IsItemDeactivatedAfterEdit()) {
							value = std::clamp(val, min, max);
							if (act.tempParametersActive[id]) data.m_tuneCallback();
						}
						ImGui::SameLine(0.0f, 20.0f);

					}
					ImGui::EndTable();
				}

			}

			ImGui::EndChild();


			if (!data.active) {
				ImGui::EndDisabled();
			}
		}
		ImGui::EndGroup();
		ImGui::EndChild();
	}
};
#include "pch.hpp"

#include "Widgets.hpp"

namespace JFMApp::Views {
	void Widgets::PlottingArea(Data::PlotData& data) {
		if (!data.characteristics || !data.paramConfig) return;

		//plot and list are in the table
		ImVec2 tableSize = ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y * 0.5f);
		ImGuiTableFlags tableFlags = ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersOuter;

		if (ImGui::BeginTable("PlottingArea", 2, tableFlags, tableSize)) {
			ImGui::TableSetupColumn("Plot");
			ImGui::TableSetupColumn("List");

			ImGui::TableNextRow();

			ImGui::TableSetColumnIndex(0);
			ImGui::SetColumnWidth(0, ImGui::GetContentRegionAvail().x * 0.75f);
			//draw the plot area
			{


				ImVec2 plotAreaSize = ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y);
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
							ImPlot::PlotLine(act.name.c_str(), act.V.data() + act.dataRange.first, act.I.data() + act.dataRange.first, act.dataRange.second - act.dataRange.first);
						}

						//fitted characteristics
						if (act.isFitted && data.plotFitted) {
							ImPlot::SetNextLineStyle(data.colorFitted);
							ImPlot::PlotLine(act.name.c_str(), act.V.data() + act.dataRange.first, act.fittedI.data(), act.dataRange.second - act.dataRange.first);
						}

						//tuned characteristics
						if (act.isFitted && data.plotFitted) {
							ImPlot::SetNextLineStyle(data.colorFitted, 2.0);
							ImPlot::PlotLine(act.name.c_str(), act.V.data() + act.dataRange.first, act.tunedI.data(), act.dataRange.second - act.dataRange.first);
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
								ImPlot::PlotLine(ch.name.c_str(), ch.V.data() + ch.dataRange.first, ch.I.data() + ch.dataRange.first, ch.dataRange.second - ch.dataRange.first);
							}

							//fitted characteristics
							if (ch.isFitted && data.plotFitted) {
								ImPlot::SetNextLineStyle(data.colorFitted);
								ImPlot::PlotLine(ch.name.c_str(), ch.V.data() + ch.dataRange.first, ch.fittedI.data(), ch.dataRange.second - ch.dataRange.first);
							}

							//tuned characteristics
							if (ch.isFitted && data.plotFitted) {
								ImPlot::SetNextLineStyle(data.colorFitted, 2.0);
								ImPlot::PlotLine(ch.name.c_str(), ch.V.data() + ch.dataRange.first, ch.tunedI.data(), ch.dataRange.second - ch.dataRange.first);
							}

						}
					}

					//plot infinite lines

					if(data.active){
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

			ImGui::TableSetColumnIndex(1);
			ImGui::SetColumnWidth(1, ImGui::GetContentRegionAvail().x);
			//draw the characteristic list
			{
				ImVec2 listSize = ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y);
				if (ImGui::BeginListBox("List", listSize)) {
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
			ImGui::ColorEdit4("Ranged Color", &data.colorRanged.x);

			ImGui::SameLine(0.0, 20.0);
			ImGui::Checkbox("Plot Fitted", &data.plotFitted);
			ImGui::SameLine();
			ImGui::ColorEdit4("Fitted Color", &data.colorFitted.x);

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

		//draw the characteristic settings
		{

			int lower = 0;
			int middle0 = std::max(lower, (int)act.dataRange.second - 1);
			int middle1 = std::max(middle0 + 1, (int)act.dataRange.first + 1);
			int upper = std::max(middle1, (int)act.V.size() - 1);
			groupH = ImGui::GetCursorPosY();
			ImGui::BeginGroup();

			auto vStr = data.active ? std::to_string(act.V[act.dataRange.first]) : "0";

			float sliderW = ImGui::GetCursorPosX();

			if (ImGui::SliderInt("Up range", (int*)&(act.dataRange.first), middle1, upper, vStr.c_str())) {

			}

			sliderW = ImGui::GetCursorPosX() - sliderW;

			ImGui::SameLine(0.0, 50.0);

			float buttonW = ImGui::GetCursorPosX();

			if (ImGui::Button("Estimate")) {
				data.m_estimateCallback();
			}

			buttonW = ImGui::GetCursorPosX() - buttonW;

			vStr = data.active ? std::to_string(act.V[act.dataRange.second]) : "0";

			ImGui::PushItemWidth(sliderW);

			if (ImGui::SliderInt("Down range", (int*)&(act.dataRange.first), lower, middle0, vStr.c_str())) {

			}

			ImGui::PopItemWidth();

			ImGui::SameLine(0.0, 50.0);
			ImGui::PushItemWidth(buttonW);

			if (ImGui::Button("Fit")) {
				data.m_fitCallback();
			}

			ImGui::PopItemWidth();

			ImGui::EndGroup();
			groupH = ImGui::GetCursorPosY() - groupH;
		}

		ImGui::SameLine(0.0, 30.0);

		//draw the parameter tables
		{
			ImGuiTableFlags tableFlags = ImGuiTableFlags_Borders
				| ImGuiTableFlags_Resizable
				| ImGuiTableFlags_Reorderable
				| ImGuiTableFlags_Hideable;

			ImVec2 tableSize = ImVec2(ImGui::GetContentRegionAvail().x, groupH);

			if (ImGui::BeginTable("Parameters", 2, tableFlags, tableSize)) {

				ImGui::TableSetupColumn("Fitted");
				ImGui::TableSetupColumn("Tuned");

				ImGui::TableHeadersRow();

				ImGui::TableNextRow();

				if (ImGui::BeginTable("Fitted", 2, tableFlags & ~ImGuiTableFlags_Reorderable)) {
					ImGui::TableSetupColumn("Parameter");
					ImGui::TableSetupColumn("Value");


					for (auto& [id, value] : act.fittedParameters) {
						ImGui::TableNextRow();
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

					for (auto& [id, value] : act.tunedParameters) {
						ImGui::TableNextRow();
						ImGui::Text(data.paramConfig->parameters[id].c_str());
						ImGui::TableNextColumn();
						ImGui::Text(std::to_string(value).c_str());
					}

					ImGui::EndTable();
				}

				ImGui::EndTable();
			}

		}

		//draw the parameter sliders
		{
			if (ImGui::BeginTable("Tuned parameters", 3)) {
				ImGui::TableNextRow();
				for (auto& [id, value] : act.tunedParameters) {
					float val = value;
					float min = data.paramConfig->paramBounds[id].first;
					float max = data.paramConfig->paramBounds[id].second;

					ImGui::TableNextColumn();
					ImGui::Checkbox(data.paramConfig->parameters[id].c_str(), &act.tempParametersActive[id]);
					ImGui::SameLine(0.0, 20.0);
					if(ImGui::SliderFloat(data.paramConfig->parameters[id].c_str(), &val, min, max, "%e")){
						value = val;
						if(act.tempParametersActive[id]) data.m_tuneCallback();
					}

				}
			}

		}

		ImGui::Separator();



		ImGui::Text("Tune Error: %e", act.tuneError >= 0.0 ? act.tuneError : 0.0);

		ImGui::SameLine(ImGui::GetContentRegionAvail().x * 0.8f);

		if (ImGui::Button("Update characteristic")) {
			for (auto& [key, val] : act.fittedParameters) {
				if(act.tempParametersActive[key]) val = act.tunedParameters[key];
			}

			act.m_tuneCallback();
		}

		if (!data.active) {
			ImGui::EndDisabled();
		}
	}
};
#include "pch.hpp"

#include "Widgets.hpp"

namespace JFMApp::Views {
	void Widgets::PlottingArea(Data::PlotData* data) {
		if (!data || !data->characteristics) return;

		//draw the plot area

		ImVec2 plotAreaSize = ImVec2(ImGui::GetContentRegionAvail().x * 0.9, ImGui::GetContentRegionAvail().y * 0.5);
		if (ImPlot::BeginPlot("Characteristics", plotAreaSize, data->plotSettings.flags)) {

			ImPlot::SetupAxes("V", "I", data->plotSettings.xFlags, data->plotSettings.yFlags);

			if (data->logX)
				ImPlot::SetupAxisScale(ImAxis_X1, Data::Characteristic::TFL, Data::Characteristic::TFNL);

			if (data->logY)
				ImPlot::SetupAxisScale(ImAxis_Y1, Data::Characteristic::TFL, Data::Characteristic::TFNL);


			auto& act = *(data->active);
			if (data->active && data->hideNonActive) {
				//main characteristics
				ImPlot::PlotLine("Active", act.V.data(), act.I.data(), std::min(act.V.size(), act.I.size()));
				//ranged characteristics

				//fitted characteristics
			}
			else if (!data->hideNonActive && data->characteristics) {
				//main characteristics
				for (auto& ch : *(data->characteristics))
					ImPlot::PlotLine(ch.name.c_str(), ch.V.data(), ch.I.data(), std::min(ch.V.size(), ch.I.size()));

				//ranged characteristics

				//fitted characteristics
			}

			ImPlot::EndPlot();
		}


		//-----------------
		//draw the characteristic list



		//-----------------

		//draw the plot settings

		//-----------------

		//draw the characteristic settings

		//-----------------
	}
};
#include "pch.hpp"

#include "Widgets.hpp"

namespace JFMApp::Views {

	static std::vector<std::array<double, 3>> chi2vec = {
		{1.0, 4.0, 9.0},
		{2.30, 6.18, 11.8},
		{3.53, 8.02, 14.2},
		{4.72, 9.72, 16.3},
		{5.89, 11.3, 18.2},
		{7.04, 12.8, 20.1}
	};

	static int InputTextCallback(ImGuiInputTextCallbackData* data)
	{
		std::string* str = (std::string*)data->UserData;
		if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
		{
			str->resize(data->BufTextLen);
			data->Buf = (char*)str->c_str();
		}
		return 0;
	}

	static size_t getMCColor(double error, size_t dof) {
		size_t i = 0;

		auto& vec = chi2vec[dof - 1];

		if (error > vec[0])
			i = 1;
		if (error > vec[1])
			i = 2;

		return i;
	}

	static void drawPlot(Data::PlotData::MCPlotsData& mc, Data::NumericsConfig& nConf) {
		if (!mc.characteristic) return;
		ImGui::ColorPicker4("Sigma 1", &mc.sig[0].x, ImGuiColorEditFlags_NoInputs);
		ImGui::SameLine();
		ImGui::ColorPicker4("Sigma 2", &mc.sig[1].x, ImGuiColorEditFlags_NoInputs);
		ImGui::SameLine();
		ImGui::ColorPicker4("Sigma 3", &mc.sig[2].x, ImGuiColorEditFlags_NoInputs);

		ImGui::SameLine(0.0f, 20.0f);

		std::string& prX = nConf.parameters[mc.parameters.first];

		auto& params = nConf.modelParameters[mc.characteristic->modelID];

		if (ImGui::BeginCombo("X", prX.c_str())) {

			for (auto& param : params) {
				if (param != mc.parameters.second)
					ImGui::Selectable(nConf.parameters[param].c_str(), param == mc.parameters.first);
			}

			ImGui::EndCombo();
		}

		ImGui::SameLine();

		std::string& prY = nConf.parameters[mc.parameters.second];

		if (ImGui::BeginCombo("Y", prY.c_str())) {

			for (auto& param : params) {
				if (param != mc.parameters.first)
					ImGui::Selectable(nConf.parameters[param].c_str(), param == mc.parameters.second);
			}

			ImGui::EndCombo();
		}


		ImVec2 plotAreaSize = ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y);
		if (ImPlot::BeginPlot(mc.name.c_str(), plotAreaSize, Data::PlotData::plotSettings.flags)) {

			ImPlot::SetupAxes(prX.c_str(), prY.c_str(), Data::PlotData::plotSettings.xFlags, Data::PlotData::plotSettings.yFlags);

			for (auto& d : mc.mc) {
				ImPlot::SetNextLineStyle(mc.sig[getMCColor(d.error, d.fixConfig.size())]);
				ImPlot::PlotScatter("MC", &d.parameters[mc.parameters.first], &d.parameters[mc.parameters.second], 1);
			}

			ImPlot::EndPlot();
		}

	}


	void Widgets::MonteCarloInspector(Data::PlotData& data) {
		if (!data.paramConfig) return;
		auto& nConf = *(data.paramConfig);


		ImGuiTabBarFlags tabFlags = ImGuiTabBarFlags_NoTooltip
			| ImGuiTabBarFlags_FittingPolicyScroll;

		if (ImGui::BeginTabBar("MC Tabs", tabFlags)) {


			if (ImGui::TabItemButton("Add", ImGuiTabItemFlags_Trailing | ImGuiTabItemFlags_NoTooltip)) {

			}

			for (auto& tab : data.mcTabs) {
				std::string name = "MC group" + std::to_string(tab);
				if (ImGui::BeginTabItem(name.c_str(), nullptr, ImGuiTabItemFlags_None))
				{
					ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x * 0.8f);
					ImGui::BeginGroup();


					{
						auto& name = data.mcTempName;

						ImGui::InputTextWithHint("##Name", "Name",
							(char*)name.c_str(),
							name.capacity() + 1,
							ImGuiInputTextFlags_CallbackResize,
							InputTextCallback,
							(void*)&name);

						ImGui::SameLine();

						auto& tempParams = data.mcTempParams;

						if (data.activeMC) {
							auto& mc = *(data.activeMC);

							std::string& prX = nConf.parameters[tempParams.first];

							auto& params = nConf.modelParameters[data.activeMC->characteristic->modelID];

							if (ImGui::BeginCombo("X", prX.c_str())) {

								for (auto& param : params) {
									if (param != tempParams.second)
										ImGui::Selectable(nConf.parameters[param].c_str(), param == mc.parameters.first);
								}

								ImGui::EndCombo();
							}

							ImGui::SameLine();

							std::string& prY = nConf.parameters[tempParams.second];

							if (ImGui::BeginCombo("Y", prY.c_str())) {

								for (auto& param : params) {
									if (param != tempParams.first)
										ImGui::Selectable(nConf.parameters[param].c_str(), param == mc.parameters.second);
								}

								ImGui::EndCombo();
							}
						}

						ImGui::SameLine();

						bool cond = data.activeMC && data.activeMC->characteristic;

						auto str = std::find_if(data.mcPlots.begin(), data.mcPlots.end(), [&](const auto& mc) {
							return mc.name == data.mcTempName;
							});

						if (str != data.mcPlots.end()) cond = false;

						if (!cond)
							ImGui::BeginDisabled();

						if (ImGui::Button("Add plot")) {

						}

						if (!cond)
							ImGui::EndDisabled();


					}

					//dockspace
					{

						ImGuiChildFlags window_flags = ImGuiChildFlags_Border;

						ImVec2 child_size = ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y);

						ImGui::BeginChild("##dockspace", child_size);

						ImGuiID dockspace_id = ImGui::GetID(tab + 1);
						ImGui::DockSpace(dockspace_id, child_size, ImGuiDockNodeFlags_None);

						ImGui::EndChild();
					}

					ImGui::EndGroup();

					ImGui::PopItemWidth();

					ImGui::SameLine();

					{
						ImVec2 listSize = ImVec2(ImGui::GetContentRegionAvail().x * 0.2f, ImGui::GetContentRegionAvail().y);

						if (ImGui::BeginListBox("List", listSize) && data.characteristics) {
							for (auto& ch : *(data.characteristics)) {
								if (!ch.checked) continue;

								ImGui::Text(ch.name.c_str());
								ImGui::Separator();

								for (auto& mc : data.mcPlots) {
									if (mc.characteristic == &ch && ImGui::Selectable(ch.name.c_str(), data.activeMC == &mc)) {
										data.activeMC = &mc;

										auto& tempParams = data.mcTempParams;

										tempParams.first = nConf.modelParameters[mc.characteristic->modelID][0];
										tempParams.second = nConf.modelParameters[mc.characteristic->modelID][1];
									}

								}

							}
							ImGui::EndListBox();
						}
					}

					ImGui::EndTabItem();
				}
				else {
					ImGuiChildFlags child_flags = ImGuiChildFlags_Border;
					ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse;

					ImVec2 child_size = ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y);

					ImGui::BeginChild("##dockspace", child_size, child_flags, window_flags);

					ImGuiID dockspace_id = ImGui::GetID(tab + 1);
					ImGui::DockSpace(dockspace_id, child_size, ImGuiDockNodeFlags_KeepAliveOnly);

					ImGui::EndChild();
				}

				{
					for (auto& mc : data.mcPlots) {
						if (mc.tab == tab) {
							ImGui::SetNextWindowDockID(ImGui::GetID(tab + 1), ImGuiCond_FirstUseEver);
							ImGui::Begin(mc.name.c_str(), nullptr, ImGuiWindowFlags_NoCollapse);
								drawPlot(mc, nConf);
							ImGui::End();
						}
					}
				}
			}

			ImGui::EndTabBar();
		}
	}
};
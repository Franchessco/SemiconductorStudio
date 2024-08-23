#include "pch.hpp"

#include "Widgets.hpp"

namespace JFMApp::Views {
	void Widgets::ViewMenu(Data::UIState& data) {
		
		if (ImGui::BeginMenuBar()) {

			if (ImGui::BeginMenu("Views")) {

				ImGui::MenuItem("Plotting Area", NULL, &data.m_showPlottingArea);

				ImGui::MenuItem("Browser Area", NULL, &data.m_showBrowserArea);
					
				ImGui::MenuItem("Characteristic Inspector", NULL, &data.m_showCharacteristicInspector);

				ImGui::MenuItem("Monte Carlo Inspector", NULL, &data.m_showMonteCarloInspector);
					
				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}
	}
}
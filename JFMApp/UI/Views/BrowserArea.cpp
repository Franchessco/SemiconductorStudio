#include "pch.hpp"

#include "Widgets.hpp"

namespace JFMApp::Views {
	void Widgets::BrowserArea(Data::BrowserData& data) {
		ImGui::BeginGroup();
		CharacteristicList(data);

		FileSelector(data);
		ImGui::EndGroup();
		
	}
};
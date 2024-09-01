#include "pch.hpp"

#include "Widgets.hpp"

namespace JFMApp::Views {
	void Widgets::BrowserArea(Data::BrowserData& data) {
		if(ImGui::Begin("Characteristic List"))
			CharacteristicList(data);

		ImGui::End();
	
		if (ImGui::Begin("File Explorer"))
			FileSelector(data);

		ImGui::End();
		
	}
};
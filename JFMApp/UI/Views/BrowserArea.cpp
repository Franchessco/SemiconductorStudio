#include "pch.hpp"

#include "Widgets.hpp"

namespace JFMApp::Views {
	void Widgets::BrowserArea(Data::BrowserData& data) {

		CharacteristicList(data);

		FileSelector(data);
		
	}
};
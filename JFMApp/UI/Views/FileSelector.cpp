#include "pch.hpp"

#include "Widgets.hpp"

namespace JFMApp::Views {
	void Widgets::FileSelector(Data::BrowserData& data)
	{
		if (ImGui::BeginChild("Browser")) {

			if (ImGui::BeginMenuBar()) {
				if (ImGui::Button("<-")) {
					data.m_selection.clear();
					data.currentPath = data.currentPath.parent_path();
					data.m_selection.resize(std::distance(std::filesystem::directory_iterator(data.currentPath), std::filesystem::directory_iterator{}));
				}

				if (ImGui::Button("Load")) {

				}

				if (ImGui::Button("Load all")) {

				}

				ImGui::EndMenuBar();
			}


			float width = ImGui::GetContentRegionAvail().x;
			float size = 100.0f;
			int numItems = std::min((int)(width / size), 1);

			ImVec2 selSize = ImVec2(size, size);

			if (ImGui::BeginTable("Browser", numItems), ImGuiTableFlags_BordersOuter) {
				//put selectables in here

				ImGui::TableNextRow();

				size_t sel_index{ 0 };

				for (auto& entry : std::filesystem::directory_iterator(data.currentPath)) {
					std::string name = entry.path().filename().string();

					if (ImGui::Selectable(name.c_str(), data.m_selection[sel_index++], 0, selSize)) {
						data.m_selection[sel_index - 1] = !data.m_selection[sel_index - 1];
					}

					//if the entry is clicked
					//  if the entry is a directory -> selected, all other cleared
					//	if ctrl pressed
					//		if the entry is a file -> selected
					//	if ctrl not pressed
					//		if the entry is a file -> selected, all other cleared
					//
					//if the entry is double clicked
					//	if the entry is a directory -> current path = entry.path(), all selection cleared
					//	if the entry is a file -> callback


					if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
						if (!ImGui::GetIO().KeyCtrl || entry.is_directory()) {
							for (const auto& sel : data.m_selection)
								sel = false;

							data.m_selection[sel_index - 1] = true;
						}
					}



					if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {

						if (entry.is_directory()) {
							data.currentPath = entry.path();
							data.m_selection.clear();
							data.m_selection.resize(std::distance(std::filesystem::directory_iterator(data.currentPath), std::filesystem::directory_iterator{}));


						}
						else {
							for (const auto& sel : data.m_selection)
								sel = false;

							data.m_selection[sel_index - 1] = true;

							//place for your callback
						}

					}

				}

				ImGui::TableNextColumn();
			}


			ImGui::EndChild();
		}
	}
}
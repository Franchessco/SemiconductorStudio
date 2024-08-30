#include "pch.hpp"

#include "Widgets.hpp"

namespace JFMApp::Views {
	void Widgets::global_ds()
	{

		ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		mDS = ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), dockspace_flags);

		/*ImGuiWindowFlags window_flags = 0;
		window_flags |= ImGuiWindowFlags_NoTitleBar;
		window_flags |= ImGuiWindowFlags_NoScrollbar;
		window_flags |= ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoResize;
		window_flags |= ImGuiWindowFlags_NoCollapse;
		window_flags |= ImGuiWindowFlags_NoNav;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;

		const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x, main_viewport->WorkPos.y));
		ImGui::SetNextWindowSize(ImVec2(main_viewport->WorkSize.x, main_viewport->WorkSize.y));
		bool open = true;
		ImGui::Begin("##", &open, window_flags);

		ImGui::End();*/
	}
};
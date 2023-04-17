// Author: Alex Hartford
// Program: Engine
// File: Debug
// Date: March 2023

#ifndef DEBUG_H
#define DEBUG_H

void DebugUI() {
    ImGuiWindowFlags flags = 0;
    flags |= ImGuiWindowFlags_NoBackground;
    flags |= ImGuiWindowFlags_NoTitleBar;
    flags |= ImGuiWindowFlags_NoTitleBar;
	flags |= ImGuiWindowFlags_NoCollapse;
	flags |= ImGuiWindowFlags_NoInputs;
	flags |= ImGuiWindowFlags_NoMove;
	flags |= ImGuiWindowFlags_NoScrollbar;
	flags |= ImGuiWindowFlags_NoResize;

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(SCREEN_WIDTH, SCREEN_HEIGHT));

    ImGui::Begin("Debug", NULL, flags);
    {
        ImGui::Text("[%.2f FPS]", ImGui::GetIO().Framerate);
    }
    ImGui::End();
}

#endif

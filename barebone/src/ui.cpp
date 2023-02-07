#include "ui.h"

#include "common/log.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>

#include "imgui.h"
namespace {
  bool ui_show_demo_window = false;
}

//---------------------------------------------
// Entry UI function
void ui_show() {
  //Set an initial window size and position
  ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_Once);
  ImGui::SetNextWindowSize(ImVec2(1000.0f, 800.0f), ImGuiCond_Once);

  ImGui::Begin("Welcome");

  if (ui_show_demo_window) ImGui::ShowDemoWindow(&ui_show_demo_window);
  ImGui::Checkbox("Show Demo Window?", &ui_show_demo_window);

  ImGui::Text("Time %.1f", ImGui::GetTime());

  ImGui::Text("Hello World");

  ImGui::End();
}


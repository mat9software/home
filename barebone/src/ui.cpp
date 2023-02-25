#include "ui.h"

#include "common/log.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>

#include "imgui.h"
#include "emscripten.h"
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

  if (ImGui::Button("Load Ini File")) {
    ImGui::LoadIniSettingsFromDisk("/offline/imgui.ini");
    EM_ASM(FS.syncfs(function(err) {
      if (err) {
        console.log("cache_put : Sync failed" + err);
      }
    }););
  }
  if (ImGui::Button("Save Ini File")) {
    ImGui::SaveIniSettingsToDisk("/offline/imgui.ini");
    EM_ASM(FS.syncfs(function(err) {
      if (err) {
        console.log("cache_put : Sync failed" + err);
      }
    }););
  }

  ImGui::End();
}


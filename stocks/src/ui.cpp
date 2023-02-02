#include "ui.h"

#include <stdio.h>

#include "fetch.h"
#include "imgui.h"
namespace {
  std::vector<float> graph_values = {0.6f, 0.1f, 1.0f, 0.5f, 0.92f, 0.1f, 0.2f};
  float graph_min = -1.0f;
  float graph_max = 1.0f;

  std::function<void(const std::vector<float>)> graph_cb =
    [](const std::vector<float> vec) {
      graph_values = vec;
      const auto min = *min_element(vec.begin(), vec.end());
      const auto max = *max_element(vec.begin(), vec.end());
      graph_min = min - (abs(min)*0.1);
      graph_max = max + (abs(max)*0.1);
    };
}


void ui_show() {
  static bool show_demo_window = true;

  if (show_demo_window) ImGui::ShowDemoWindow(&show_demo_window);

  ImGui::Begin("Stocks");

  ImGui::Text("This is some useful text.");
  if (ImGui::Button("Button")) {
    fetch1(graph_cb);
  }

  ImGui::Text("Time %.1f", ImGui::GetTime());

  int stride = sizeof(float);
  ImGui::PlotLines("Lines", graph_values.data(), graph_values.size(), 0.0f, nullptr, graph_min,
                   graph_max, ImVec2(0, 80.0f), stride);

  ImGui::End();
}

/*
void ui_show()
{
    static bool show_demo_window = true;
    static bool show_another_window = false;

    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);

    {
        static float f = 0.0f;
        static int counter = 0;

        ImGui::Begin("Hello, world!");

        ImGui::Text("This is some useful text.");
        ImGui::Text("Test");
        ImGui::Checkbox("Demo Window", &show_demo_window);
        ImGui::Checkbox("Another Window", &show_another_window);

        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);

        if (ImGui::Button("Button"))
            counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        ImGui::Text("Hola Carlos");
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f /
ImGui::GetIO().Framerate, ImGui::GetIO().Framerate); ImGui::Text("Super Time
%.1f", ImGui::GetTime());

// Draw a Rectangle primitive
        //ImDrawList* draw_list = ImGui::GetWindowDrawList();
        //ImVec4 colf = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        //const ImU32 col = ImColor(colf);
//
        //ImVec2 p0 = ImGui::GetCursorScreenPos();
        //ImVec2 p1 = ImVec2(p0.x + 200.0f, p0.y + 200.0f);
        //draw_list->AddRectFilled(p0, p1, col, 10.0f);
//
        static float values[] = { 0.6f, 0.1f, 1.0f, 0.5f, 0.92f, 0.1f, 0.2f };
        ImGui::PlotLines("Lines", values, IM_ARRAYSIZE(values), 0.0f, nullptr,
-1.0f, 1.0f, ImVec2(0, 80.0f));


        ImGui::End();
    }

    if (show_another_window)
    {
        show_stocks();
    }
}
*/

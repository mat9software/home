#include "ui.h"

#include "common/log.h"
#include <stdio.h>

#include "fetch.h"
#include "imgui.h"
namespace {
//TODO 
//     1- Use vector of graph_data to scale many stock graph. 
//     each one with a close button and input text.
//
//     2- Interval time will be global and update all graph at once.
  struct graph_data {
     std::vector<float> values = {};
     float min = -1.0f;
     float max = 1.0f;
     char stock_symbol[128] = "aapl";
  };
  std::vector<graph_data> graph_datas;

  std::vector<float> graph_values = {};
  float graph_min = -1.0f;
  float graph_max = 1.0f;
  char stock_symbol[128] = "aapl";

//---------------------------------------------
  std::function<void(const std::vector<float>)> graph_cb =
    [](const std::vector<float> vec) {
      graph_values = vec;
      const auto min = *min_element(vec.begin(), vec.end());
      const auto max = *max_element(vec.begin(), vec.end());
      graph_min = min - (abs(min)*0.1);
      graph_max = max + (abs(max)*0.1);
    };
}

//---------------------------------------------
void ui_show() {
  static bool show_demo_window = false;

  if (show_demo_window) ImGui::ShowDemoWindow(&show_demo_window);

  ImGui::Begin("Stocks");

  ImGui::Checkbox("Show Demo Window?", &show_demo_window);

  ImGui::InputText("Stock Symbol", stock_symbol, IM_ARRAYSIZE(stock_symbol));

  if (ImGui::Button("Load")) {
    fetch1({.symbol_name = stock_symbol, .success_cb = graph_cb});
  }

  ImGui::Text("Time %.1f", ImGui::GetTime());

  int stride = sizeof(float);
  ImGui::PlotLines("Lines", graph_values.data(), graph_values.size(), 0.0f, nullptr, graph_min,
                   graph_max, ImVec2(0, 200.0f), stride);

  ImGui::End();
}


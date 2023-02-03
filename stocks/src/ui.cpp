#include "ui.h"

#include "common/log.h"
#include "data.h"
#include <stdio.h>

#include "fetch.h"
#include "imgui.h"
namespace {
  std::vector<graph_data> graph_datas;

  std::function<void(const graph_add& add)> graph_cb =
    [](const graph_add& add) {
      const auto& vec = add.values;
      graph_datas[add.index].values = vec;
      const auto min = *min_element(vec.begin(), vec.end());
      const auto max = *max_element(vec.begin(), vec.end());
      graph_datas[add.index].min = min - (abs(min)*0.1);
      graph_datas[add.index].max = max + (abs(max)*0.1);
    };

//---------------------------------------------
  int graph_nb = 2;
  char graph_range[10] = "100d";
  char graph_interval[10] = "1d";

  bool ui_show_demo_window = false;
}

//---------------------------------------------
void graph_show(graph_data& curr) {
    ImGui::InputText("Stock", curr.stock_symbol, IM_ARRAYSIZE(curr.stock_symbol));

    ImGui::SameLine();
    if (ImGui::Button("Load")) {
      fetch1({.data=curr,.range=graph_range,.interval=graph_interval});
    }

    int stride = sizeof(float);
    ImGui::PlotLines("Lines", curr.values.data(), curr.values.size(), 0.0f, nullptr, curr.min,
                     curr.max, ImVec2(0, 200.0f), stride);
}
//---------------------------------------------
void header_show() {
  if (ui_show_demo_window) ImGui::ShowDemoWindow(&ui_show_demo_window);
  ImGui::Checkbox("Show Demo Window?", &ui_show_demo_window);

  //ImGui::Text("Time %.1f", ImGui::GetTime());

  ImGui::InputText("Range", graph_range, IM_ARRAYSIZE(graph_range));
  ImGui::InputText("Interval", graph_interval, IM_ARRAYSIZE(graph_interval));

  if (ImGui::Button("Add a graph")) {
    graph_nb++;
  }
  ImGui::SameLine();
  if (ImGui::Button("Remove a graph")) {
    graph_nb--;
  }
}
//---------------------------------------------
void ui_show() {

  while(graph_datas.size() < graph_nb) {
    graph_datas.push_back({.index=graph_datas.size(), .update_graph_cb=graph_cb });
  }

  ImGui::Begin("Stocks");

  header_show();

  for(int i = 0; i < graph_nb; i++) {
    ImGui::PushID(i);

    graph_show(graph_datas[i]);

    ImGui::PopID();
  }

  ImGui::End();
}


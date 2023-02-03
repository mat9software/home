#include "ui.h"

#include "common/log.h"
#include "data.h"
#include <stdio.h>

#include "fetch.h"
#include "imgui.h"
namespace {
  std::vector<const char*> graph_valid_range = {"5d","1mo","3mo","6mo","1y","2y","5y","10y","ytd","max"};
//---------------------------------------------
  std::vector<graph_data> graph_datas;
  int graph_nb = 2;
  float graph_min = -1.0f;
  float graph_max = -1.0f;
  bool graph_same_min_max = false;
  char graph_range[10] = "100d";
  int graph_range_idx = 0;
  char graph_interval[10] = "1d";

//---------------------------------------------
  bool ui_show_demo_window = false;

//---------------------------------------------
  std::function<void(const graph_add& add)> graph_cb =
    [](const graph_add& add) {
      const auto& vec = add.values;
      graph_datas[add.index].values = vec;
      const auto min = *min_element(vec.begin(), vec.end());
      const auto max = *max_element(vec.begin(), vec.end());
      graph_datas[add.index].min = min - (abs(min)*0.1);
      graph_datas[add.index].max = max + (abs(max)*0.1);
      
      if(graph_min == -1.0f) graph_min = min;
      if(graph_max == -1.0f) graph_max = max;
       
      if(graph_min > min) graph_min = min;
      if(graph_max < max) graph_max = max;
    };
}

//---------------------------------------------
void graph_show(graph_data& curr) {
    ImGui::InputText("Stock", curr.stock_symbol, IM_ARRAYSIZE(curr.stock_symbol));

    int stride = sizeof(float);
    if(graph_same_min_max) {
        ImGui::PlotLines(curr.stock_symbol, curr.values.data(), curr.values.size(), 0.0f, nullptr, graph_min,
                         graph_max, ImVec2(0, 200.0f), stride);
    } else {
        ImGui::PlotLines(curr.stock_symbol, curr.values.data(), curr.values.size(), 0.0f, nullptr, curr.min,
                         curr.max, ImVec2(0, 200.0f), stride);
    }
}
//---------------------------------------------
void header_show() {
  if (ui_show_demo_window) ImGui::ShowDemoWindow(&ui_show_demo_window);
  ImGui::Checkbox("Show Demo Window?", &ui_show_demo_window);

  //ImGui::Text("Time %.1f", ImGui::GetTime());

  ImGui::Checkbox("Same Graph Range for All", &graph_same_min_max);

  //mdtmp ImGui::InputText("Range", graph_range, IM_ARRAYSIZE(graph_range));
  ImGui::Text("Range : %s", graph_valid_range[graph_range_idx]);

  if (ImGui::ArrowButton("##left", ImGuiDir_Left)) {
    graph_range_idx--;
    if(graph_range_idx < 0) graph_range_idx = 0;
  }
  //mdtmp ImGui::SameLine(0.0f, spacing);
  ImGui::SameLine();
  if (ImGui::ArrowButton("##right", ImGuiDir_Right)) {
    graph_range_idx++;
    if(graph_range_idx >= graph_valid_range.size()) graph_range_idx = graph_valid_range.size()-1;
  }

  //mdtmp ImGui::InputText("Range", graph_range, IM_ARRAYSIZE(graph_range));
  ImGui::InputText("Interval", graph_interval, IM_ARRAYSIZE(graph_interval));
  if (ImGui::Button("Load")) {
    graph_min = -1.0f;
    graph_max = -1.0f;

    for(int i = 0; i < graph_nb; i++) {
      fetch_stock({.data=graph_datas[i],.range=graph_valid_range[graph_range_idx],.interval=graph_interval});
    }
  }

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


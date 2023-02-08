#include "ui.h"

#include "common/log.h"
#include "data.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>

#include "fetch.h"
#include "imgui.h"
namespace {
//That the APP Data
//---------------------------------------------
  int graph_range_idx = 6;
  const std::vector<const char*> graph_valid_range = {"5d","1mo","3mo","6mo","1y","2y","5y","10y","ytd","max"};
//---------------------------------------------
  std::vector<graph_data> graph_datas;
//---------------------------------------------
  float graph_shared_min = -1.0f;
  float graph_shared_max = -1.0f;
  bool graph_same_min_max = false;
//---------------------------------------------
  char graph_stocks_input[200] = "aapl";
//---------------------------------------------
  bool ui_show_demo_window = false;

/*
//mdtmp
Add fetch_data{
on_success;
on_failure;
graph_index;
std::vector<fetch_data> curr_request;
int waiting_for_request; // must be 0 to enable button. Put to nb when launching it
*/

//---------------------------------------------
  std::function<void(const graph_add& add)> graph_cb =
    [](const graph_add& add) {
//mdtmp only populate values.
//mdtmp find another place to calculate min/max etc.. poucentage
//mdtmp add ui and data to zoom in and move across values.
      //Update grap_data for index.
      const auto& vec = add.values;
      graph_datas[add.index].values = vec;
      const auto min = *min_element(vec.begin(), vec.end());
      const auto max = *max_element(vec.begin(), vec.end());
      graph_datas[add.index].min = min - (abs(min)*0.1);
      graph_datas[add.index].max = max + (abs(max)*0.1);
      
      //Keep track of global min/max.
      if(graph_shared_min == -1.0f) graph_shared_min = min;
      if(graph_shared_max == -1.0f) graph_shared_max = max;
       
      if(graph_shared_min > min) graph_shared_min = min;
      if(graph_shared_max < max) graph_shared_max = max;
    };
}

//---------------------------------------------
void graph_show(graph_data& curr) {
    if(graph_same_min_max) {
        ImGui::PlotLines(curr.stock_symbol, curr.values.data(), curr.values.size(), 0.0f, nullptr, graph_shared_min,
                         graph_shared_max, ImVec2(0, 200.0f));
    } else {
        ImGui::PlotLines(curr.stock_symbol, curr.values.data(), curr.values.size(), 0.0f, nullptr, curr.min,
                         curr.max, ImVec2(0, 200.0f));
    }
}
//---------------------------------------------
void header_show() {
  if (ui_show_demo_window) ImGui::ShowDemoWindow(&ui_show_demo_window);
  ImGui::Checkbox("Show Demo Window?", &ui_show_demo_window);

  //ImGui::Text("Time %.1f", ImGui::GetTime());

  ImGui::Checkbox("Same Range for All Graph", &graph_same_min_max);

  ImGui::InputText("Stocks Input", graph_stocks_input, IM_ARRAYSIZE(graph_stocks_input));

  ImGui::Text("Range : %s", graph_valid_range[graph_range_idx]);

  if (ImGui::ArrowButton("##left", ImGuiDir_Left)) {
    graph_range_idx--;
    if(graph_range_idx < 0) graph_range_idx = 0;
  }

  ImGui::SameLine();
  if (ImGui::ArrowButton("##right", ImGuiDir_Right)) {
    graph_range_idx++;
    if(graph_range_idx >= graph_valid_range.size()) graph_range_idx = graph_valid_range.size()-1;
  }

  if (ImGui::Button("Load")) {
    // Reset global data.
    graph_shared_min = -1.0f;
    graph_shared_max = -1.0f;

    { // Get all stock from a ',| ' separated string.

      std::vector<const char*> split = str_split_delimiter(graph_stocks_input, {',',' '});
      size_t word_nb = 0;
      for (int j = 0; j < split.size(); j += 2) {
        const char* itr = split[j];
        const char* end = split[j + 1];

        word_nb++;
        size_t i = word_nb - 1;
        graph_datas.resize(word_nb);
        graph_datas[i] = {.index = i, .update_graph_cb = graph_cb};

        size_t size = end - itr;
        if (size < STOCK_SYMBOL_LENGTH) {
          memcpy(&graph_datas[i].stock_symbol, itr, size);
          memset(&graph_datas[i].stock_symbol[size], 0, 1);
        } else {
          graph_datas[i].stock_symbol[0] = 0;
          LOG_ERROR("String to long");
        }
      }
    }

    for(int i = 0; i < graph_datas.size(); i++) {
      fetch_stock({.data=graph_datas[i],.range=graph_valid_range[graph_range_idx]});
    }
  }
}

//---------------------------------------------
// Entry UI function
void ui_show() {
  //Set an initial window size and position
  ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_Once);
  ImGui::SetNextWindowSize(ImVec2(1000.0f, 800.0f), ImGuiCond_Once);

  ImGui::Begin("Stocks");

  header_show();

  for(int i = 0; i < graph_datas.size(); i++) {
    //PushID to differenciate similar UI component.
    ImGui::PushID(i);

    graph_show(graph_datas[i]);

    ImGui::PopID();
  }

  ImGui::End();
}


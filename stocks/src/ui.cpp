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

//---------------------------------------------
  std::vector<fetch_args> curr_requests;
  // States for waiting_for_requests 
  // -1 : Ready to Load new requests.
  //  0 : Data received, calculating statistic from data.
  // >0 : Request in progress. Waiting for result.
  // -2 : Starting Request, Load is disable.
  int waiting_for_requests = -1;

//---------------------------------------------
  void on_done() {
    for(auto& it:graph_datas) {
      auto& vec = it.values;

      const auto min = *min_element(vec.begin(), vec.end());
      const auto max = *max_element(vec.begin(), vec.end());
      it.min = min - (abs(min)*0.1); // add 10% on graph value.
      it.max = max + (abs(max)*0.1); // add 10% on graph value.
      
      //Keep track of global min/max.
      if(graph_shared_min == -1.0f) graph_shared_min = min;
      if(graph_shared_max == -1.0f) graph_shared_max = max;
       
      if(graph_shared_min > min) graph_shared_min = min;
      if(graph_shared_max < max) graph_shared_max = max;
    }

    waiting_for_requests = -1;
  }

  std::function<void(const graph_cb_args& add)> on_success_cb =
    [](const graph_cb_args& add) {
      //Receive stocks data.
      const auto& vec = add.values;
      graph_datas[add.index].values = vec;

      waiting_for_requests--;
      if (waiting_for_requests == 0) {
        on_done();
      }
    };
 
    std::function<void(const graph_cb_args& add)> on_failure_cb =
    [](const graph_cb_args& add) {
      LOG_ERROR("Request failed. Index %zu", add.index);
      waiting_for_requests--;
      if (waiting_for_requests == 0) {
        on_done();
      }
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

  ImGui::BeginDisabled(waiting_for_requests != -1);
  if (ImGui::Button("Load")) {
    waiting_for_requests = -2; //Disable Load button.
    curr_requests.clear();

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

    waiting_for_requests = graph_datas.size();
    for (size_t i = 0; i < graph_datas.size(); i++) {
      curr_requests.push_back({
          .index = i,
          .range = graph_valid_range[graph_range_idx],
          .stock_symbol = graph_datas[i].stock_symbol,
          .on_success = on_success_cb,
          .on_failure = on_failure_cb,
      });
    }
    for(auto& it:curr_requests) {
      fetch_stock(it);
    }
  }
  ImGui::EndDisabled();
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


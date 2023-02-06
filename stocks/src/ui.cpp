#include "ui.h"

#include "common/log.h"
#include "data.h"
#include <stdio.h>
#include <string.h>

#include "fetch.h"
#include "imgui.h"
namespace {
  std::vector<const char*> graph_valid_range = {"5d","1mo","3mo","6mo","1y","2y","5y","10y","ytd","max"};
//---------------------------------------------
  std::vector<graph_data> graph_datas;
  float graph_min = -1.0f;
  float graph_max = -1.0f;
  bool graph_same_min_max = false;
  int graph_range_idx = 0;
  char graph_interval[10] = "1d";
  char graph_stocks_input[200] = "aapl";

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

  ImGui::InputText("Stocks Input", graph_stocks_input, IM_ARRAYSIZE(graph_stocks_input));

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

  ImGui::InputText("Interval", graph_interval, IM_ARRAYSIZE(graph_interval));
  if (ImGui::Button("Load")) {
    graph_min = -1.0f;
    graph_max = -1.0f;

    { // Get all stock from a ',| ' separated string.

      LOG_TRACE("Testing algorithm");
      LOG_TRACE("%s", graph_stocks_input);
      char* end = graph_stocks_input;
      while (*end != 0) {
        end++;
      }

      size_t nb = 0;
      char* itr = graph_stocks_input;
      char* begin = itr;
      while (true) {
        if (*itr == ' ' || *itr == ',' || itr == end) {
          if (itr == begin) {
            // empty word
            if (itr == end) break;

            itr++;
            begin = itr;
            continue;
          }

          // found a word
          nb++;
          size_t i = nb-1;
          graph_datas.resize(nb);
          graph_datas[i] = {.index=i, .update_graph_cb=graph_cb };

          size_t size = itr - begin;
          LOG_TRACE("size:%zu",size);
          LOG_TRACE("begin:%p",begin);
          if (size < STOCK_SYMBOL_LENGTH) {
            memcpy(&graph_datas[i].stock_symbol, begin, size);
            memset(&graph_datas[i].stock_symbol[size], 0, 1);
          } else {
            graph_datas[i].stock_symbol[0] = 0;
            LOG_ERROR("String to long");
          }
          LOG_TRACE("after mem cpy : %s",graph_datas[i].stock_symbol);


          if (itr == end) break;
          itr++;
          begin = itr;
          continue;
        }

        itr++;
      }
    }
    for(int i = 0; i < graph_datas.size(); i++ ) {
      LOG_TRACE("After algo : %s", graph_datas[i].stock_symbol);
    }

    for(int i = 0; i < graph_datas.size(); i++) {
      fetch_stock({.data=graph_datas[i],.range=graph_valid_range[graph_range_idx],.interval=graph_interval});
    }
  }

}
//---------------------------------------------
void ui_show() {
  ImGui::Begin("Stocks");

  header_show();

  for(int i = 0; i < graph_datas.size(); i++) {
    ImGui::PushID(i);

    graph_show(graph_datas[i]);

    ImGui::PopID();
  }

  ImGui::End();
}


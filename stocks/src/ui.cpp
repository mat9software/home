#include "ui.h"

#include <stdio.h>
#include <string.h>

#include "cache.h"
#include "common/log.h"
#include "data.h"
#include "fetch.h"
#include "imgui.h"
#include "implot.h"
#include "utils.h"

namespace {
// The APP Data
//---------------------------------------------
int graph_range_idx = 6;
const std::vector<const char*> graph_valid_range = {
    "5d", "1mo", "3mo", "6mo", "1y", "2y", "5y", "10y", "ytd", "max"};
int graph_range_start_offset = 0;
int graph_range_end_offset = 0;

//---------------------------------------------
std::vector<graph_data> graph_datas;
std::vector<float> graph_x_datas;
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
  for (auto& it : graph_datas) {
    auto& vec = it.values;

    const auto min = *min_element(vec.begin(), vec.end());
    const auto max = *max_element(vec.begin(), vec.end());
    it.min = min - (abs(min) * 0.1);  // add 10% on graph value.
    it.max = max + (abs(max) * 0.1);  // add 10% on graph value.

    if (!vec.empty()) {
      it.ratio_begin_to_end = vec[vec.size() - 1] / vec[0];
    }
    it.ratio_min_to_max = max / min;

    it.ratio_values.clear();
    it.ratio_values.reserve(vec.size());
    if (!vec.empty()) {
      for (auto& v : vec) {
        it.ratio_values.push_back(v / vec[0] * 100.f);
      }
    }

    // Keep track of global min/max.
    if (graph_shared_min == -1.0f) graph_shared_min = min;
    if (graph_shared_max == -1.0f) graph_shared_max = max;

    if (graph_shared_min > min) graph_shared_min = min;
    if (graph_shared_max < max) graph_shared_max = max;
  }

  if (!graph_datas.empty()) {
    const size_t size = graph_datas[0].values.size();
    if (size <= 0) {
      LOG_ERROR("Values Size of first request is empty : %zu", size);
    }
    graph_x_datas.resize(size);
    float now = (float)time(0);
    float time_value = now;
    float every_five_days = 0;
    for (int i = size - 1; i >= 0; i--, every_five_days++) {
      graph_x_datas[i] = time_value;
      time_value -= 24.f * 3600.f;
      if (every_five_days >= 4) {
        every_five_days = 0;
        // Approximation algorithm to skip the weekend.
        time_value -= 24.f * 3600.f;
        time_value -= 24.f * 3600.f;
      }
    }
  }

  waiting_for_requests = -1;
}

std::function<void(const graph_cb_args& add)> on_success_cb =
    [](const graph_cb_args& add) {
      // Receive stocks data.
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
}  // namespace

//---------------------------------------------
void implot_show() {
  if (graph_datas.empty()) {
    return;
  }
  if (ImPlot::BeginPlot("Stocks (Abs)")) {
    ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Time);
    for (auto& it : graph_datas) {
      if (it.values.empty()) continue;
      if (graph_range_start_offset + graph_range_end_offset >=
          it.values.size()) {
        LOG_ERROR(
            "Offset invalid. Offset greater then size. offset:%d|size:%zu",
            graph_range_start_offset + graph_range_end_offset,
            it.values.size());
      }

      ImPlot::PlotLine(it.stock_symbol,
                       &graph_x_datas.data()[graph_range_start_offset],
                       &it.values.data()[graph_range_start_offset],
                       it.values.size() - graph_range_end_offset - graph_range_start_offset);
    }
    ImPlot::EndPlot();
  }
  if (ImPlot::BeginPlot("Stocks (Pourcentage)")) {
    ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Time);
    for (auto& it : graph_datas) {
      if (it.ratio_values.empty()) continue;
      if (graph_range_start_offset + graph_range_end_offset >=
          it.ratio_values.size()) {
        LOG_ERROR(
            "Offset invalid. Offset greater then size. offset:%d|size:%zu",
            graph_range_start_offset + graph_range_end_offset,
            it.values.size());
      }

      ImPlot::PlotLine(it.stock_symbol,
                       &graph_x_datas.data()[graph_range_start_offset],
                       &it.ratio_values.data()[graph_range_start_offset],
                       it.values.size() - graph_range_end_offset - graph_range_start_offset);
    }
    ImPlot::EndPlot();
  }
}

//---------------------------------------------
void header_show() {
  if (ui_show_demo_window) ImGui::ShowDemoWindow(&ui_show_demo_window);
  ImGui::Checkbox("Show Demo Window?", &ui_show_demo_window);

  // ImGui::Text("Time %.1f", ImGui::GetTime());

  ImGui::Checkbox("Same Range for All Graph", &graph_same_min_max);

  ImGui::InputText("Stocks Input", graph_stocks_input,
                   IM_ARRAYSIZE(graph_stocks_input));

  {
    ImGui::Text("Range : %s", graph_valid_range[graph_range_idx]);
    ImGui::SameLine();
    if (ImGui::ArrowButton("##left_range", ImGuiDir_Left)) {
      graph_range_idx--;
      if (graph_range_idx < 0) graph_range_idx = 0;
    }
    ImGui::SameLine();
    if (ImGui::ArrowButton("##right_range", ImGuiDir_Right)) {
      graph_range_idx++;
      if (graph_range_idx >= graph_valid_range.size())
        graph_range_idx = graph_valid_range.size() - 1;
    }
  }
  {
    ImGui::Text("Start Offset : %d", graph_range_start_offset);
    ImGui::SameLine();
    if (ImGui::ArrowButton("##left_range_start_offset_100", ImGuiDir_Left)) {
      graph_range_start_offset -= 100;
      if (graph_range_start_offset < 0) graph_range_start_offset = 0;
    }
    ImGui::SameLine();
    if (ImGui::ArrowButton("##left_range_start_offset_10", ImGuiDir_Left)) {
      graph_range_start_offset -= 10;
      if (graph_range_start_offset < 0) graph_range_start_offset = 0;
    }
    ImGui::SameLine();
    if (ImGui::ArrowButton("##left_range_start_offset", ImGuiDir_Left)) {
      graph_range_start_offset--;
      if (graph_range_start_offset < 0) graph_range_start_offset = 0;
    }
    ImGui::SameLine();
    if (ImGui::ArrowButton("##right_range_start_offset", ImGuiDir_Right)) {
      graph_range_start_offset++;
    }
    ImGui::SameLine();
    if (ImGui::ArrowButton("##right_range_start_offset_10", ImGuiDir_Right)) {
      graph_range_start_offset += 10;
    }
    ImGui::SameLine();
    if (ImGui::ArrowButton("##right_range_start_offset_100", ImGuiDir_Right)) {
      graph_range_start_offset += 100;
    }
  }
  {
    ImGui::Text("End Offset : %d", graph_range_end_offset);
    ImGui::SameLine();
    if (ImGui::ArrowButton("##left_range_end_offset_100", ImGuiDir_Left)) {
      graph_range_end_offset -= 100;
      if (graph_range_end_offset < 0) graph_range_end_offset = 0;
    }
    ImGui::SameLine();
    if (ImGui::ArrowButton("##left_range_end_offset_10", ImGuiDir_Left)) {
      graph_range_end_offset -= 10;
      if (graph_range_end_offset < 0) graph_range_end_offset = 0;
    }
    ImGui::SameLine();
    if (ImGui::ArrowButton("##left_range_end_offset", ImGuiDir_Left)) {
      graph_range_end_offset--;
      if (graph_range_end_offset < 0) graph_range_end_offset = 0;
    }
    ImGui::SameLine();
    if (ImGui::ArrowButton("##right_range_end_offset", ImGuiDir_Right)) {
      graph_range_end_offset++;
    }
    ImGui::SameLine();
    if (ImGui::ArrowButton("##right_range_end_offset_10", ImGuiDir_Right)) {
      graph_range_end_offset += 10;
    }
    ImGui::SameLine();
    if (ImGui::ArrowButton("##right_range_end_offset_100", ImGuiDir_Right)) {
      graph_range_end_offset += 100;
    }
  }

  ImGui::BeginDisabled(waiting_for_requests != -1);
  if (ImGui::Button("Load")) {
    waiting_for_requests = -2;  // Disable Load button.
    curr_requests.clear();
    graph_datas.clear();

    // Reset global data.
    graph_shared_min = -1.0f;
    graph_shared_max = -1.0f;

    {  // Get all stock from a ',| ' separated string.

      std::vector<const char*> split =
          str_split_delimiter(graph_stocks_input, {',', ' '});
      for (int j = 0; j < split.size(); j += 2) {
        const char* itr = split[j];
        const char* end = split[j + 1];

        graph_data to_add;

        size_t size = end - itr;
        if (size < STOCK_SYMBOL_LENGTH) {
          memcpy(&to_add.stock_symbol, itr, size);
          memset(&to_add.stock_symbol[size], 0, 1);
        } else {
          to_add.stock_symbol[0] = 0;
          LOG_ERROR("String to long");
        }
        graph_datas.push_back(to_add);
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
    for (auto& it : curr_requests) {
      fetch_stock(it);
    }
  }
  ImGui::EndDisabled();

  if (ImGui::Button("Clean Cache")) {
    cache_rm_all();
    graph_datas.clear();
  }
}

//---------------------------------------------
void ui_init() { ImPlot::GetStyle().UseLocalTime = true; }

//---------------------------------------------
// Entry UI function
void ui_show() {
  // Set an initial window size and position
  ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_Once);
  ImGui::SetNextWindowSize(ImVec2(1000.0f, 800.0f), ImGuiCond_Once);

  ImGui::Begin("Stocks");

  header_show();

  implot_show();

  ImGui::End();
}

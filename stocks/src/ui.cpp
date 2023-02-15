#include "ui.h"

#include <stdio.h>
#include <string.h>

#include "cache.h"
#include "common/log.h"
#include "common/assert.h"
#include "data.h"
#include "fetch.h"
#include "imgui.h"
#include "implot.h"
#include "utils.h"

namespace {
// The APP Data
//---------------------------------------------
bool error_code_422 = false;
//---------------------------------------------
int graph_range_idx = 0;
const std::array<const char*, 11> graph_valid_range = {
    "1d", "5d", "1mo", "3mo", "6mo", "1y", "2y", "5y", "10y", "ytd", "max"};

int graph_interval_idx = 0;
const std::array<const char*, 9> graph_valid_interval = {"2m", "5m", "15m", "30m", "1h", "1d", "1w", "1m", "1y"};

//---------------------------------------------
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
void fill_date(std::vector<float>& dates, const char* interval_str) {
    float time_increment_sec;
    float jump_increment_sec;
    float jump_threshold_iteration;

    // Stock Market opened 6.5 hours.
    const float OPEN_MINUTES = 6.5f * 60.f;
    if(strcmp("1m", interval_str) == 0) {
      time_increment_sec = 60.f;
      jump_increment_sec = 17.5 * 3600.f;
      jump_threshold_iteration = OPEN_MINUTES;
    }
    else if(strcmp("2m", interval_str) == 0) {
      time_increment_sec = 60.f * 2.f;
      jump_increment_sec = 17.5 * 3600.f;
      jump_threshold_iteration = OPEN_MINUTES / 2.f;
    }
    else if(strcmp("5m", interval_str) == 0) {
      time_increment_sec = 60.f * 5.f;
      jump_increment_sec = 17.5 * 3600.f;
      jump_threshold_iteration = OPEN_MINUTES / 5.f;
    }
    else if(strcmp("15m", interval_str) == 0) {
      time_increment_sec = 60.f * 15.f;
      jump_increment_sec = 17.5 * 3600.f;
      jump_threshold_iteration = OPEN_MINUTES / 15.f;
    }
    else if(strcmp("30m", interval_str) == 0) {
      time_increment_sec = 60.f * 30.f;
      jump_increment_sec = 17.5 * 3600.f;
      jump_threshold_iteration = OPEN_MINUTES / 30.f;
    }
    else if(strcmp("1h", interval_str) == 0) {
      time_increment_sec = 3600.f;
      jump_increment_sec = 17.5 * 3600.f;
      jump_threshold_iteration = OPEN_MINUTES / 60.f;
    }
    else if(strcmp("4h", interval_str) == 0) {
      time_increment_sec = 3600.f * 4.f;
      jump_increment_sec = 17.5 * 3600.f;
      jump_threshold_iteration = OPEN_MINUTES / 60.f / 4.f;
    }
    else if(strcmp("1d", interval_str) == 0) {
      time_increment_sec = 24.f * 3600.f;
      jump_increment_sec = 2.f * 24.f * 3600.f;
      jump_threshold_iteration = 5;
    }
    else if(strcmp("1w", interval_str) == 0) {
      time_increment_sec = 24.f * 3600.f * 7.f;
      jump_increment_sec = 0.f;
      jump_threshold_iteration = -1.f;
    }
    else if(strcmp("1m", interval_str) == 0) {
      time_increment_sec = 24.f * 3600.f * 30.5f;
      jump_increment_sec = 0.f;
      jump_threshold_iteration = -1.f;
    }
    else if(strcmp("1y", interval_str) == 0) {
      time_increment_sec = 24.f * 3600.f * 364.75f;
      jump_increment_sec = 0.f;
      jump_threshold_iteration = -1.f;
    }
    else {
      ASSERT(0);
    }

    const float NOW = (float)time(0);
    float time_value = NOW;
    float jump_iteration = 1;
    for (int i = dates.size() - 1; i >= 0; i--, jump_iteration++) {
      dates[i] = time_value;
      time_value -= time_increment_sec;
      if (jump_iteration >= jump_threshold_iteration) {
        jump_iteration = 1;
        // Approximation algorithm to skip the weekend or off-hours.
        time_value -= jump_increment_sec;
      }
    }
}

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
    fill_date(graph_x_datas, graph_valid_interval[graph_interval_idx]);
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

std::function<void(const graph_cb_args& add, int error_code)> on_failure_cb =
    [](const graph_cb_args& add, int error_code) {
      LOG_ERROR("Request failed. Index %zu", add.index);
      waiting_for_requests--;
      if (waiting_for_requests == 0) {
        on_done();
      }
      if(error_code == 422) {
        error_code_422 = true;
      }
    };
}  // namespace

//---------------------------------------------
void implot_show() {
  if (graph_datas.empty()) {
    return;
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
    ImGui::Text("Interval : %s", graph_valid_interval[graph_interval_idx]);
    ImGui::SameLine();
    if (ImGui::ArrowButton("##left_interval", ImGuiDir_Left)) {
      graph_interval_idx--;
      if (graph_interval_idx < 0) graph_interval_idx = 0;
    }
    ImGui::SameLine();
    if (ImGui::ArrowButton("##right_interval", ImGuiDir_Right)) {
      graph_interval_idx++;
      if (graph_interval_idx >= graph_valid_interval.size())
        graph_interval_idx = graph_valid_interval.size() - 1;
    }
  }
  {
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
            .interval = graph_valid_interval[graph_interval_idx],
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

#ifdef ENABLE_CACHE
  if (ImGui::Button("Clean Cache")) {
    cache_rm_all();
    graph_datas.clear();
  }
#endif //ENABLE_CACHE
}

//---------------------------------------------
void error_show() {
  if (error_code_422) {
    ImGui::OpenPopup("Fetch Error : 422");
    error_code_422 = false;
  }

  if (ImGui::BeginPopupModal("Fetch Error : 422", NULL, ImGuiWindowFlags_AlwaysAutoResize))
  {
      ImGui::Text("Relation between Range and Interval is invalid.\nReceived 422 error code from Yahoo finance request.");
      if (ImGui::Button("Close", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
      ImGui::EndPopup();
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

  error_show();

  ImGui::End();
}

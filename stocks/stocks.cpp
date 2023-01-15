#include "stocks.h"

#include "imgui.h"

void show_stocks()
{
   ImGui::Begin("Stocks");

     static float values[] = { 0.6f, 0.1f, 1.0f, 0.5f, 0.92f, 0.1f, 0.2f };
     ImGui::PlotLines("Lines", values, IM_ARRAYSIZE(values), 0.0f, nullptr, -1.0f, 1.0f, ImVec2(0, 80.0f));

   ImGui::End();
}

#include "ui.h"

#include "stocks.h"
#include "imgui.h"

void ui_show()
{
    static bool show_demo_window = true;
    static bool show_another_window = false;
    // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);

    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
    {
        static float f = 0.0f;
        static int counter = 0;

        ImGui::Begin("Hello, world!");                                // Create a window called "Hello, world!" and append into it.

        ImGui::Text("This is some useful text.");                     // Display some text (you can use a format strings too)
        ImGui::Text("Test");
        ImGui::Checkbox("Demo Window", &show_demo_window);            // Edit bools storing our window open/close state
        ImGui::Checkbox("Another Window", &show_another_window);

        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);                  // Edit 1 float using a slider from 0.0f to 1.0f

        if (ImGui::Button("Button"))                                  // Buttons return true when clicked (most widgets return true when edited/activated)
            counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        ImGui::Text("Hola Carlos");
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::Text("Super Time %.1f", ImGui::GetTime());

// Draw a Rectangle primitive
/*
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        ImVec4 colf = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        const ImU32 col = ImColor(colf);

        ImVec2 p0 = ImGui::GetCursorScreenPos();
        ImVec2 p1 = ImVec2(p0.x + 200.0f, p0.y + 200.0f);
        draw_list->AddRectFilled(p0, p1, col, 10.0f);
*/

        static float values[] = { 0.6f, 0.1f, 1.0f, 0.5f, 0.92f, 0.1f, 0.2f };
        ImGui::PlotLines("Lines", values, IM_ARRAYSIZE(values), 0.0f, nullptr, -1.0f, 1.0f, ImVec2(0, 80.0f));


        ImGui::End();
    }

    // 3. Show another simple window.
    if (show_another_window)
    {
        show_stocks();
    }
}

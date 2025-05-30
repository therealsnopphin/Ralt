#include "pch.h"

void menulayer::OnAttach()
{
    FT_Init_FreeType(&menulayer::s_currentft);
    FT_New_Face(menulayer::s_currentft, "Resource/PixelOperatorMono-Bold.ttf", 0, &menulayer::s_currentface);

    FT_Set_Pixel_Sizes(menulayer::s_currentface, 16, 16);
}

void menulayer::OnUIRender()
{
    if (menulayer::scene_menulayer && !playlayer::scene_playlayer)
    {
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImVec2 windowPos = viewport->Pos;
        ImVec2 windowSize = viewport->Size;

        ImGui::SetNextWindowPos(windowPos);
        ImGui::SetNextWindowSize(windowSize);

        ImGui::Begin("menulayer: ", nullptr,
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoScrollbar |
            ImGuiWindowFlags_NoScrollWithMouse |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoDocking);

        if (ImGui::Button("Start game"))
        {
            if (s_playername != "")
            {
                playlayer::scene_playlayer = true;
                menulayer::scene_menulayer = false;
            }
  
        }

        ImGui::InputText("Enter name", &s_playername);

        ImGui::End();
    }
}

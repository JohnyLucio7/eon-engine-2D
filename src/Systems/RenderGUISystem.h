//
// Created by lucio on 04/01/2026.
//

#ifndef EON_ENGINE_2D_RENDERGUISYSTEM_H
#define EON_ENGINE_2D_RENDERGUISYSTEM_H

#include "../ECS/ECS.h"
#include <imgui/imgui.h>
#include <imgui/imgui_sdl.h>

class RenderGUISystem : public System {
public:
    RenderGUISystem() = default;

    void Update() {
        ImGui::NewFrame();

        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoNav |
                                       ImGuiWindowFlags_AlwaysAutoResize;
        if (ImGui::Begin("Spawn enemies", nullptr, windowFlags)) {
            ImGui::Text("This windows should be fixed and with no decoration...");
        }
        ImGui::End();

        ImGui::Render();
        ImGuiSDL::Render(ImGui::GetDrawData());
    }
};

#endif //EON_ENGINE_2D_RENDERGUISYSTEM_H

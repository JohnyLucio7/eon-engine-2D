//
// Created by lucio on 04/01/2026.
//

#ifndef EON_ENGINE_2D_RENDERGUISYSTEM_H
#define EON_ENGINE_2D_RENDERGUISYSTEM_H

#include "../ECS/ECS.h"
#include <imgui/imgui.h>
#include <imgui/imgui_sdl.h>
#include <glm/glm.hpp>
#include <cmath>
#include "../Components/TransformComponent.h"
#include "../Components/RigidbodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/ProjectileEmitterComponent.h"
#include "../Components/HealthComponent.h"

class RenderGUISystem : public System {
public:
    RenderGUISystem() = default;

    void Update(const std::unique_ptr<Registry> &registry, const SDL_Rect &camera) {
        ImGui::NewFrame();

        // Janela principal de Spawn
        if (ImGui::Begin("Spawn enemies")) {
            // Static variables (Mantendo estado entre frames)
            static int enemyXPos = 0;
            static int enemyYPos = 0;
            static float enemyScaleX = 1.0f;
            static float enemyScaleY = 1.0f;
            static float enemyRotation = 0.0f;
            static float rbVelX = 0.0f;
            static float rbVelY = 0.0f;
            static int enemyHealth = 100;

            // Sprites
            const char *sprites[] = {"tank-image", "truck-image", "radar-image", "chopper-image"};
            static int selectedSpriteIndex = 0;
            static int spriteWidth = 32;
            static int spriteHeight = 32;
            static int spriteZIndex = 1;

            // Collider
            static int colliderWidth = 32;
            static int colliderHeight = 32;

            // Projectile (Usando segundos para facilitar a UX)
            static float projAngle = 0.0f;
            static float projSpeed = 100.0f;
            static float projRepeatSec = 2.0f; // Padrão 2 segundos (mais rápido de ver)
            static float projDurationSec = 5.0f;
            static int projDamage = 10;
            static bool projIsFriendly = false;

            // --- SEÇÃO TRANSFORM ---
            if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::InputInt("Pos X", &enemyXPos);
                ImGui::InputInt("Pos Y", &enemyYPos);
                ImGui::SliderFloat("Scale X", &enemyScaleX, 0.1f, 10.0f);
                ImGui::SliderFloat("Scale Y", &enemyScaleY, 0.1f, 10.0f);
                ImGui::SliderFloat("Rotation", &enemyRotation, 0.0f, 360.0f);
            }

            // --- SEÇÃO SPRITE ---
            if (ImGui::CollapsingHeader("Sprite", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::Combo("Texture ID", &selectedSpriteIndex, sprites, IM_ARRAYSIZE(sprites));
                ImGui::InputInt("Width", &spriteWidth);
                ImGui::InputInt("Height", &spriteHeight);
                ImGui::InputInt("Z Index", &spriteZIndex);
            }

            // --- SEÇÃO RIGIDBODY ---
            if (ImGui::CollapsingHeader("Rigidbody", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::InputFloat("Vel X", &rbVelX);
                ImGui::InputFloat("Vel Y", &rbVelY);
            }

            // --- SEÇÃO COLLIDER ---
            if (ImGui::CollapsingHeader("Collider", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::InputInt("Col Width", &colliderWidth);
                ImGui::InputInt("Col Height", &colliderHeight);
            }

            // --- SEÇÃO PROJECTILE ---
            if (ImGui::CollapsingHeader("Projectile Emitter", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::SliderFloat("Proj Angle", &projAngle, 0.0f, 360.0f);
                ImGui::InputFloat("Proj Speed", &projSpeed);
                ImGui::InputFloat("Repeat (sec)", &projRepeatSec); // Input em Segundos
                ImGui::InputFloat("Duration (sec)", &projDurationSec); // Input em Segundos
                ImGui::InputInt("Damage", &projDamage);
                ImGui::Checkbox("Is Friendly", &projIsFriendly);
            }

            // --- SEÇÃO HEALTH ---
            if (ImGui::CollapsingHeader("Health", ImGuiTreeNodeFlags_DefaultOpen)) {
                ImGui::SliderInt("HP %", &enemyHealth, 0, 100);
            }

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            // --- LÓGICA DE SPAWN ---
            if (ImGui::Button("Spawn new enemy")) {
                Entity enemy = registry->CreateEntity();
                enemy.Group("enemies");

                enemy.AddComponent<TransformComponent>(
                    glm::vec2(enemyXPos, enemyYPos),
                    glm::vec2(enemyScaleX, enemyScaleY),
                    enemyRotation
                );

                enemy.AddComponent<RigidbodyComponent>(
                    glm::vec2(rbVelX, rbVelY)
                );

                enemy.AddComponent<SpriteComponent>(
                    sprites[selectedSpriteIndex],
                    spriteWidth,
                    spriteHeight,
                    spriteZIndex
                );

                enemy.AddComponent<BoxColliderComponent>(
                    colliderWidth,
                    colliderHeight
                );

                // Conversão de Graus para Vetor
                double rad = glm::radians(projAngle);
                glm::vec2 projVelocity;
                projVelocity.x = std::cos(rad) * projSpeed;
                projVelocity.y = std::sin(rad) * projSpeed;

                // Conversão de Segundos para Milissegundos (* 1000)
                enemy.AddComponent<ProjectileEmitterComponent>(
                    projVelocity,
                    static_cast<int>(projRepeatSec * 1000),
                    static_cast<int>(projDurationSec * 1000),
                    projDamage,
                    projIsFriendly
                );

                enemy.AddComponent<HealthComponent>(enemyHealth);
            }
        }
        ImGui::End();

        // --- JANELA DE COORDENADAS (Do Professor - Muito útil para Debug) ---
        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
                                       ImGuiWindowFlags_NoNav;
        ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always, ImVec2(0, 0));
        ImGui::SetNextWindowBgAlpha(0.9f);
        if (ImGui::Begin("Map coordinates", NULL, windowFlags)) {
            ImGui::Text(
                "Map coordinates (x=%.1f, y=%.1f)",
                ImGui::GetIO().MousePos.x + camera.x,
                ImGui::GetIO().MousePos.y + camera.y
            );
        }
        ImGui::End();


        ImGui::Render();
        ImGuiSDL::Render(ImGui::GetDrawData());
    }
};
#endif //EON_ENGINE_2D_RENDERGUISYSTEM_H

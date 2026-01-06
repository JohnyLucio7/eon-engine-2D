//
// Created by lucio on 03/01/2026.
//

#ifndef EON_ENGINE_2D_CAMERAMOVEMENTSYSTEM_H
#define EON_ENGINE_2D_CAMERAMOVEMENTSYSTEM_H

#include "../ECS/ECS.h"
#include "../Components/CameraFollowComponent.h"
#include "../Components/TransformComponent.h"
#include "SDL2/SDL.h"

class CameraMovementSystem : public System {
public:
    CameraMovementSystem() {
        RequireComponent<CameraFollowComponent>();
        RequireComponent<TransformComponent>();
    }

    void Update(SDL_Rect &camera) {
        for (auto entity: GetSystemEntities()) {
            auto transform = entity.GetComponent<TransformComponent>();

            // CORREÇÃO: Usar camera.w e camera.h (Resolução Lógica) em vez de Game::windowWidth/Height (Janela Física)
            if (transform.position.x + (camera.w / 2) < Game::mapWidth) {
                camera.x = transform.position.x - (camera.w / 2);
            }

            if (transform.position.y + (camera.h / 2) < Game::mapHeight) {
                camera.y = transform.position.y - (camera.h / 2);
            }

            // Keep camera rectangle view inside the screen limits
            camera.x = camera.x < 0 ? 0 : camera.x;
            camera.y = camera.y < 0 ? 0 : camera.y;
            camera.x = camera.x > camera.w ? camera.w : camera.x;
            camera.y = camera.y > camera.h ? camera.h : camera.y;
        }
    }
};
#endif //EON_ENGINE_2D_CAMERAMOVEMENTSYSTEM_H
//
// Created by lucio on 03/01/2026.
//

#ifndef EON_ENGINE_2D_PROJECTILELIFECYCLESYSTEM_H
#define EON_ENGINE_2D_PROJECTILELIFECYCLESYSTEM_H
#include "../Components/ProjectileComponent.h"
#include "../ECS/ECS.h"
#include "SDL2/SDL.h"

class ProjectileLifecycleSystem : public System {
public:
    ProjectileLifecycleSystem() {
        RequireComponent<ProjectileComponent>();
    }

    void Update() {
        for (auto entity: GetSystemEntities()) {
            auto projectile = entity.GetComponent<ProjectileComponent>();

            if (SDL_GetTicks() - projectile.startTime > static_cast<Uint32>(projectile.duration)) {
                entity.Kill();
            }
        }
    }
};
#endif //EON_ENGINE_2D_PROJECTILELIFECYCLESYSTEM_H

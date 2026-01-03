//
// Created by lucio on 03/01/2026.
//

#ifndef EON_ENGINE_2D_PROJECTILEEMITTERCOMPONENT_H
#define EON_ENGINE_2D_PROJECTILEEMITTERCOMPONENT_H
#include <SDL2/SDL.h>
#include "glm/glm.hpp"

struct ProjectileEmitterComponent {
    glm::vec2 projectileVelocity;
    int repeatFrequency;
    int projectileDuration;
    int hitPercentDamage;
    bool isFriendly;
    int lastEmissionTime;

    ProjectileEmitterComponent(glm::vec2 projectileVelocity = glm::vec2(0), int repeatFrequency = 0,
                               int projectileDuration = 10000, int hitPercentDamage = 10, bool isFriendly = false) {
        this->projectileVelocity = projectileVelocity;
        this->repeatFrequency = repeatFrequency;
        this->projectileDuration = projectileDuration;
        this->hitPercentDamage = hitPercentDamage;
        this->isFriendly = isFriendly;
        this->lastEmissionTime = SDL_GetTicks();
    }
};

#endif //EON_ENGINE_2D_PROJECTILEEMITTERCOMPONENT_H

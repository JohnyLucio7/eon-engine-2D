//
// Created by lucio on 03/01/2026.
//

#ifndef EON_ENGINE_2D_PROJECTILECOMPONENT_H
#define EON_ENGINE_2D_PROJECTILECOMPONENT_H]
#include "SDL2/SDL.h"

struct ProjectileComponent {
    bool isFriendly;
    int hitPercentDamage;
    int duration;
    int startTime;

    ProjectileComponent(bool isFriendly = false, int hitPercentDamage = 0, int duration = 0) {
        this->isFriendly = isFriendly;
        this->hitPercentDamage = hitPercentDamage;
        this->duration = duration;
        this->startTime = SDL_GetTicks();
    }
};

#endif //EON_ENGINE_2D_PROJECTILECOMPONENT_H

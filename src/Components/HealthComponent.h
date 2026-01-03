//
// Created by lucio on 03/01/2026.
//

#ifndef EON_ENGINE_2D_HEALTHCOMPONENT_H
#define EON_ENGINE_2D_HEALTHCOMPONENT_H

struct HealthComponent {
    int healthPercentage;

    HealthComponent(int healthPercentage = 0) {
        this->healthPercentage = healthPercentage;
    }
};

#endif //EON_ENGINE_2D_HEALTHCOMPONENT_H

//
// Created by lucio on 04/01/2026.
//

#ifndef EON_ENGINE_2D_SCRIPTSYSTEM_H
#define EON_ENGINE_2D_SCRIPTSYSTEM_H
#include "../Components/ScriptComponent.h"
#include "../ECS/ECS.h"
#include "sol/sol.hpp"

class ScriptSystem : public System {
public:
    ScriptSystem() {
        RequireComponent<ScriptComponent>();
    }

    void Update() {
        for (auto entity: GetSystemEntities()) {
            const auto script = entity.GetComponent<ScriptComponent>();
            script.func();
        }
    }
};

#endif //EON_ENGINE_2D_SCRIPTSYSTEM_H

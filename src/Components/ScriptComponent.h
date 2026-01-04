//
// Created by lucio on 04/01/2026.
//

#ifndef EON_ENGINE_2D_SCRIPTCOMPONENT_H
#define EON_ENGINE_2D_SCRIPTCOMPONENT_H
#include <sol/sol.hpp>

struct ScriptComponent {
    sol::function func;

    ScriptComponent(sol::function func = sol::lua_nil) {
        this->func = func;
    }
};

#endif //EON_ENGINE_2D_SCRIPTCOMPONENT_H

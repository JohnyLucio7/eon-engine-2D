#ifndef COLLISIONSYSTEM_H
#define COLLISIONSYSTEM_H

#include "../ECS/ECS.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/TransformComponent.h"

class CollisionSystem : public System
{
public:
    CollisionSystem()
    {
        RequireComponent<TransformComponent>();
        RequireComponent<BoxColliderComponent>();
    }

    void Update()
    {
        // TODO:
        // Check all entities tha have a boxcollider
        // to see if they are colliding with each other
        // Tip: AABB collision check
    }
};

#endif /// COLLISIONSYSTEM_H
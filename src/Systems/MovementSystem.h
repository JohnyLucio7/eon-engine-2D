#ifndef MOVEMENTSYSTEM_H
#define MOVEMENTSYSTEM_H

#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidbodyComponent.h"

class MovementSystem : public System
{

public:
    MovementSystem()
    {
        RequireComponent<TransformComponent>();
        RequireComponent<RigidbodyComponent>();
    }

    void Update()
    {
        /// Loop all entities that the system is interested in
        for (auto entity : GetSystemEntities())
        {
            // Update Entity position based on its velocity
            auto &transform = entity.GetComponent<TransformComponent>();
            const auto rigidbody = entity.GetComponent<RigidbodyComponent>();

            transform.position.x += rigidbody.velocity.x;
            transform.position.y += rigidbody.velocity.y;

            Logger::Log(
                "Entity id = " +
                std::to_string(entity.GetId()) +
                " position is now (" +
                std::to_string(transform.position.x) +
                ", " +
                std::to_string(transform.position.y) +
                ")");
        }
    }
};

#endif /** MOVEMENTSYSTEM_H  */

#ifndef MOVEMENTSYSTEM_H
#define MOVEMENTSYSTEM_H

#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidbodyComponent.h"

class MovementSystem : public System {
public:
    MovementSystem() {
        RequireComponent<TransformComponent>();
        RequireComponent<RigidbodyComponent>();
    }

    void Update(double deltaTime) {
        /// Loop all entities that the system is interested in
        for (auto entity: GetSystemEntities()) {
            // Update Entity position based on its velocity
            auto &transform = entity.GetComponent<TransformComponent>();
            const auto rigidbody = entity.GetComponent<RigidbodyComponent>();

            transform.position.x += rigidbody.velocity.x * deltaTime;
            transform.position.y += rigidbody.velocity.y * deltaTime;

            bool isEntityOutsideMap = (
                transform.position.x < 0 ||
                transform.position.x > Game::mapWidth ||
                transform.position.y < 0 ||
                transform.position.y > Game::mapHeight
            );

            // Kill all entities that move outside the map boundaries
            if (isEntityOutsideMap && !entity.HasTag("player")) {
                entity.Kill();
            }
        }
    }
};

#endif /** MOVEMENTSYSTEM_H  */

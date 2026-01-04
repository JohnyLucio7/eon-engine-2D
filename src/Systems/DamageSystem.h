#ifndef DAMAGESYSTEM_H
#define DAMAGESYSTEM_H

#include "../ECS/ECS.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/ProjectileComponent.h"
#include "../Components/HealthComponent.h"
#include "../Events/CollisionEvent.h"
#include "../EventBus/EventBus.h"

class DamageSystem : public System {
public:
    DamageSystem() {
        RequireComponent<BoxColliderComponent>();
    }

    void SubscribeToEvents(std::unique_ptr<EventBus> &eventBus) {
        eventBus->SubscribeToEvent<CollisionEvent>(this, &DamageSystem::OnCollision);
    }

    void OnCollision(CollisionEvent &event) {
        Entity a = event.a;
        Entity b = event.b;
        Logger::Log("Collision Event emitted: " + std::to_string(a.GetId()) + " and " + std::to_string(b.GetId()));

        if (a.BelongsToGroup("projectiles") && b.HasTag("player")) {
            OnProjectileHitsPlayer(a, b);
        }

        if (b.BelongsToGroup("projectiles") && a.HasTag("player")) {
            OnProjectileHitsPlayer(b, a);
        }

        if (a.BelongsToGroup("projectiles") && b.BelongsToGroup("enemies")) {
            OnProjectileHitsEnemy(a, b);
        }

        if (b.BelongsToGroup("projectiles") && a.BelongsToGroup("enemies")) {
            OnProjectileHitsEnemy(b, a);
        }
    }

    void OnProjectileHitsPlayer(Entity projectile, Entity player) {
        auto projectileComponent = projectile.GetComponent<ProjectileComponent>();

        if (!projectileComponent.isFriendly) {
            auto &healthComponent = player.GetComponent<HealthComponent>();
            healthComponent.healthPercentage -= projectileComponent.hitPercentDamage;

            if (healthComponent.healthPercentage <= 0) {
                player.Kill();
            }

            projectile.Kill();
        }
    }

    void OnProjectileHitsEnemy(Entity projectile, Entity enemy) {
        auto projectileComponent = projectile.GetComponent<ProjectileComponent>();
        if (projectileComponent.isFriendly) {
            auto &healthComponent = enemy.GetComponent<HealthComponent>();
            healthComponent.healthPercentage -= projectileComponent.hitPercentDamage;

            if (healthComponent.healthPercentage <= 0) {
                enemy.Kill();
            }
            projectile.Kill();
        }
    }


    void Update() {
    }
};

#endif /// DAMAGESYSTEM_H

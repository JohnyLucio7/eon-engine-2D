#ifndef KEYBOARDCONTROLSYSTEM_H
#define KEYBOARDCONTROLSYSTEM_H

#include "../ECS/ECS.h"
#include "../EventBus/EventBus.h"
#include "../Events/KeyPressedEvent.h"
#include "../Components/KeyboardControlledComponent.h"
#include "../Components/RigidbodyComponent.h"
#include "../Components/SpriteComponent.h"

class KeyboardControlSystem : public System
{
public:
    KeyboardControlSystem()
    {
        RequireComponent<KeyboardControlledComponent>();
        RequireComponent<SpriteComponent>();
        RequireComponent<RigidbodyComponent>();
    }

    void SubscribeToEvents(std::unique_ptr<EventBus> &eventBus)
    {
        eventBus->SubscribeToEvent<KeyPreesedEvent>(this, &KeyboardControlSystem::OnKeyPressed);
    }

    void OnKeyPressed(KeyPreesedEvent &event)
    {
        for (auto entity : GetSystemEntities())
        {
            const auto keyboardControl = entity.GetComponent<KeyboardControlledComponent>();
            auto &sprite = entity.GetComponent<SpriteComponent>();
            auto &rigidbody = entity.GetComponent<RigidbodyComponent>();

            switch (event.symbol)
            {
            case SDLK_UP:
                rigidbody.velocity = keyboardControl.upVelocity;
                sprite.srcRect.y = sprite.height * 0;
                break;
            case SDLK_RIGHT:
                rigidbody.velocity = keyboardControl.rightVelocity;
                sprite.srcRect.y = sprite.height * 1;
                break;
            case SDLK_DOWN:
                rigidbody.velocity = keyboardControl.downVelocity;
                sprite.srcRect.y = sprite.height * 2;
                break;
            case SDLK_LEFT:
                rigidbody.velocity = keyboardControl.leftVelocity;
                sprite.srcRect.y = sprite.height * 3;
                break;

            default:
                break;
            }
        }
        // Todo: change the sprite and the velocity of my enitty

        // std::string keyCode = std::to_string(event.symbol);
        // std::string keySymbol(1, event.symbol);
        // Logger::Log("Key pressed event emitted: [" + keyCode + "] " + keySymbol);
    }

    void Update() {}
};

#endif /// KEYBOARDCONTROLSYSTEM_H
#ifndef KEYBOARDCONTROLSYSTEM_H
#define KEYBOARDCONTROLSYSTEM_H

#include "../ECS/ECS.h"
#include "../EventBus/EventBus.h"
#include "../Events/KeyPressedEvent.h"
#include "../Components/RigidbodyComponent.h"
#include "../Components/SpriteComponent.h"

class KeyboardControlSystem : public System
{
public:
    KeyboardControlSystem()
    {
        // RequireComponent<SpriteComponent>();
        // RequireComponent<RigidbodyComponent>();
    }

    void SubscribeToEvents(std::unique_ptr<EventBus> &eventBus)
    {
        eventBus->SubscribeToEvent<KeyPreesedEvent>(this, &KeyboardControlSystem::OnKeyPressed);
    }

    void OnKeyPressed(KeyPreesedEvent &event)
    {
        std::string keyCode = std::to_string(event.symbol);
        std::string keySymbol(1, event.symbol);
        Logger::Log("Key pressed event emitted: [" + keyCode + "] " + keySymbol);
    }

    void Update() {}
};

#endif /// KEYBOARDCONTROLSYSTEM_H
#ifndef COLLISIONEVEMT_H
#define COLLISIONEVEMT_H

#include "../ECS/ECS.h"
#include "../EventBus/Event.h"

class CollisionEvent: public Event
{
public:
    Entity a;
    Entity b;
    CollisionEvent(Entity a, Entity b) : a(a), b(b) {}
};

#endif /// COLLISIONEVEMT_H
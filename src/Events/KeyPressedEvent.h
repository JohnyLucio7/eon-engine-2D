#ifndef KEYPRESSEDEVENT_H
#define KEYPRESSEDEVENT_H

#include "../EventBus/Event.h"
#include <SDL2/SDL.h>

class KeyPreesedEvent : public Event
{
public:
    SDL_Keycode symbol;
    KeyPreesedEvent(SDL_Keycode symbol) : symbol(symbol) {}
};

#endif /// KEYPRESSEDEVENT_H
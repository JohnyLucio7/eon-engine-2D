/// @file Game.h
/// @brief Header file for the Game class that manages the core game engine functionality
/// @details Provides the main game loop, initialization, and cleanup functionality using SDL2

#ifndef GAME_H
#define GAME_H

#include "../ECS/ECS.h"
#include "../AssetStore/AssetStore.h"
#include "../EventBus/EventBus.h"
#include <SDL2/SDL.h>
#include <memory>
#include <sol/sol.hpp>

const int FPS = 60;
const int MILLISECS_PER_FRAME = 1000 / FPS;

/// @brief Main game engine class
/// @details Manages the game loop, window creation, rendering, and input processing
class Game
{
private:
    bool isRunning;
    bool isDebug;
    int millisecsPreviousFrame = 0;
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Rect camera;

    sol::state lua;

    std::unique_ptr<Registry> registry;
    std::unique_ptr<AssetStore> assetStore;
    std::unique_ptr<EventBus> eventBus;
public:
    Game();
    ~Game();
    void Initialize();
    void Run();
    void Setup();
    void ProcessInput();
    void Update();
    void Render();
    void Destroy();
    void AttachToWindow(void* handle, int width, int height);

    // Exposed to Editor
    Registry* GetRegistry() const;

    static int windowWidth;
    static int windowHeight;
    static int mapWidth;
    static int mapHeight;
};
#endif /* GAME_H */
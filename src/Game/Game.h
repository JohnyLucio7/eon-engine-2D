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
    /// @brief Constructor for the Game class
    Game();
    /// @brief Destructor for the Game class
    /// @details Ensures proper cleanup of resources
    ~Game();
    /// @brief Initializes the game engine
    /// @details Sets up SDL systems, creates window and renderer
    void Initialize();
    /// @brief Runs the main game loop
    /// @details Continuously processes input, updates game state, and renders
    void Run();
    /// @brief Initialize the game objects
    /// @details Called one time before the main game loop
    void Setup();
    /// @brief Processes user input
    /// @details Handles SDL events including window and keyboard input
    void ProcessInput();
    /// @brief Updates the game state
    /// @details Called each frame to update game logic
    void Update();
    /// @brief Renders the current game state
    /// @details Handles all drawing operations
    void Render();
    /// @brief Cleans up all game resources
    /// @details Properly destroys SDL window, renderer and quits SDL
    void Destroy();
    /// @brief Attaches the engine to an existing window handle
    /// @param handle The native window handle (void*) provided by external systems (e.g., Qt)
    /// @param width The width of the external window
    /// @param height The height of the external window
    void AttachToWindow(void* handle, int width, int height);

    static int windowWidth;
    static int windowHeight;
    static int mapWidth;
    static int mapHeight;
};
#endif /* GAME_H */
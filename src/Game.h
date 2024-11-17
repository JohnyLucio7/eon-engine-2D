/// @file Game.h
/// @brief Header file for the Game class that manages the core game engine functionality
/// @details Provides the main game loop, initialization, and cleanup functionality using SDL2

#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>

/// @brief Main game engine class
/// @details Manages the game loop, window creation, rendering, and input processing
class Game
{
private:
    bool isRunning;
    SDL_Window *window;
    SDL_Renderer *renderer;

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

    int windowWidth;
    int windowHeight;
};

#endif /* GAME_H */
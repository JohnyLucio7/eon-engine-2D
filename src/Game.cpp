/// @file Game.cpp
/// @brief Implementation of the Game class that handles core game engine functionality
/// @author Johny Lúcio Teixeira da Costa

#include "Game.h"
#include <SDL2/SDL.h>
#include <iostream>

/// @brief Constructor for the Game class
/// @details Initializes the game state to not running and logs creation
Game::Game()
{
    isRunning = false;
    std::cout << "Game constructor called!" << std::endl;
}

/// @brief Destructor for the Game class
/// @details Ensures proper cleanup and logs destruction
Game::~Game()
{
    std::cout << "Game destructor called!" << std::endl;
}

/// @brief Initializes the game engine and its core systems
/// @details Sets up SDL, creates window and renderer with default settings
void Game::Initialize()
{
    // Attempt to initialize all SDL subsystems
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        std::cerr << "[Error | Class Game | Function 'Initialize()'] - Error initializing SDL!" << std::endl;
        return;
    }

    // Create a borderless window centered on screen with 800x600 resolution

    SDL_DisplayMode displayMode;
    SDL_GetCurrentDisplayMode(0, &displayMode);

    windowWidth = 800;  // displayMode.w;
    windowHeight = 600; // displayMode.h;

    window = SDL_CreateWindow(
        NULL,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        windowWidth,
        windowHeight,
        SDL_WINDOW_BORDERLESS);

    if (!window)
    {
        std::cerr << "[Error | Class Game | Function 'Initialize()'] - Error Creating SDL window!" << std::endl;
        return;
    }

    // Initialize the renderer for the window
    renderer = SDL_CreateRenderer(window, -1, 0);

    if (!renderer)
    {
        std::cerr << "[Error | Class Game | Function 'Initialize()'] - Error Creating SDL renderer!" << std::endl;
        return;
    }

    // set fullscreen window
    //SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

    isRunning = true;
}

/// @brief Main game loop
/// @details Runs the core game loop that processes input, updates game state, and renders
void Game::Run()
{
    Setup();

    while (isRunning)
    {
        ProcessInput();
        Update();
        Render();
    }
}

/// @brief Handles all input processing
/// @details Processes SDL events including window closing and keyboard input
void Game::ProcessInput()
{
    SDL_Event sdlEvent;
    while (SDL_PollEvent(&sdlEvent))
    {
        switch (sdlEvent.type)
        {
        case SDL_QUIT:
            isRunning = false;
            break;

        case SDL_KEYDOWN:

            if (sdlEvent.key.keysym.sym == SDLK_ESCAPE)
            {
                isRunning = false;
            }

            break;

        default:
            break;
        }
    }
}

/// @brief Initialize all game objects
/// @details Configure all game objects before the first frame of main game loop
void Game::Setup()
{
    // initialize game objects
}

/// @brief Updates game state
/// @details Currently empty, will be implemented with game logic
void Game::Update()
{
    // TODO: Update game objects
}

/// @brief Renders the game state
/// @details Currently empty, will be implemented with rendering logic
void Game::Render()
{
    SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255);
    SDL_RenderClear(renderer);

    // TODO: Render all game objects
    // Draw a rectangle
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_Rect player = {10, 10, 20, 20};
    SDL_RenderFillRect(renderer, &player);

    SDL_RenderPresent(renderer);
}

/// @brief Cleanup function to properly destroy all SDL resources
/// @details Ensures proper cleanup of renderer, window, and SDL subsystems
void Game::Destroy()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
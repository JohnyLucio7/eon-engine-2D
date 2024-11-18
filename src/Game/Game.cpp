/// @file Game.cpp
/// @brief Implementation of the Game class that handles core game engine functionality
/// @author Johny Lúcio Teixeira da Costa

#include "Game.h"
#include "../Logger/Logger.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <glm/glm.hpp>

/// @brief Constructor for the Game class
/// @details Initializes the game state to not running and logs creation
Game::Game()
{
    isRunning = false;
    Logger::Log("Game constructor called!");
}

/// @brief Destructor for the Game class
/// @details Ensures proper cleanup and logs destruction
Game::~Game()
{
    Logger::Log("Game destructor called!");
}

/// @brief Initializes the game engine and its core systems
/// @details Sets up SDL, creates window and renderer with default settings
void Game::Initialize()
{
    // Attempt to initialize all SDL subsystems
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        Logger::Err("[Error | Class Game | Function 'Initialize()'] - Error initializing SDL!");
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
        Logger::Err("[Error | Class Game | Function 'Initialize()'] - Error Creating SDL window!");
        return;
    }

    // Initialize the renderer for the window
    renderer = SDL_CreateRenderer(window, -1, 0);

    if (!renderer)
    {
        Logger::Err("[Error | Class Game | Function 'Initialize()'] - Error Creating SDL renderer!");
        return;
    }

    // set fullscreen window
    // SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

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

glm::vec2 playerPosition;
glm::vec2 playerVelocity;

/// @brief Initialize all game objects
/// @details Configure all game objects before the first frame of main game loop
void Game::Setup()
{
    playerPosition = glm::vec2(10.0, 20.0);
    playerVelocity = glm::vec2(100.0, 0.0);
}

/// @brief Updates game state
/// @details Currently empty, will be implemented with game logic
void Game::Update()
{
    // TODO: Update game objects

    // If we are to fast, waste some time until we reach the MILLISECS_PER_FRAME
    int timeToWait = MILLISECS_PER_FRAME - (SDL_GetTicks() - millisecsPreviousFrame);

    if (timeToWait > 0 && timeToWait <= MILLISECS_PER_FRAME)
    {
        SDL_Delay(timeToWait);
    }

    // The Difference in ticks since the last frame, converted to seconds
    double deltaTime = (SDL_GetTicks() - millisecsPreviousFrame) / 1000.0;

    // Store the current frame time
    millisecsPreviousFrame = SDL_GetTicks();

    playerPosition.x += playerVelocity.x * deltaTime;
    playerPosition.y += playerVelocity.y * deltaTime;
}

/// @brief Renders the game state
/// @details Currently empty, will be implemented with rendering logic
void Game::Render()
{

    // Working with Double-Buffered (Back and Front) Renderer
    // All of this things be render in the back buffer
    SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255);
    SDL_RenderClear(renderer);

    // ---
    // TODO: Render all game objects
    // Draw a rectangle
    // SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    // SDL_Rect player = {10, 10, 20, 20};
    // SDL_RenderFillRect(renderer, &player);
    // ---

    // ---
    // Draw a PNG Texture
    // Load PNG Texture
    SDL_Surface *surface = IMG_Load("./assets/images/tank-tiger-right.png");
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    // <srcRect> and <dstRect>
    // <srcRect> Do you want the full texture (NULL) or just part of it (Set a Rect)?
    // <dstRect> This is the destination of our texture in the renderer
    // Copy the texture to thee renderer
    SDL_Rect dstRect = {
        static_cast<int>(playerPosition.x),
        static_cast<int>(playerPosition.y),
        32,
        32};
    SDL_RenderCopy(renderer, texture, NULL, &dstRect);
    SDL_DestroyTexture(texture);
    // ---

    // So when we call this, we swap the back buffer with the front buffer, rendering all previous designs
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
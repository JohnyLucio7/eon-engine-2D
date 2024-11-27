/// @file Game.cpp
/// @brief Implementation of the Game class that handles core game engine functionality
/// @author Johny Lúcio Teixeira da Costa

#include "Game.h"
#include "../Logger/Logger.h"
#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidbodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Systems/MovementSystem.h"
#include "../Systems/RenderSystem.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <glm/glm.hpp>
#include <fstream>

/// @brief Constructor for the Game class
/// @details Initializes the game state to not running and logs creation
Game::Game()
{
    isRunning = false;
    registry = std::make_unique<Registry>();
    assetStore = std::make_unique<AssetStore>();
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

    windowWidth = displayMode.w;
    windowHeight = displayMode.h;

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
    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

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

void Game::LoadLevel(int level)
{
    // Add the systems that need to be processed in our game
    registry->AddSystem<MovementSystem>();
    registry->AddSystem<RenderSystem>();

    // Adding assets to the asset store
    assetStore->AddTexture(renderer, "tank-image", "./assets/images/tank-panther-right.png");
    assetStore->AddTexture(renderer, "truck-image", "./assets/images/truck-ford-right.png");
    assetStore->AddTexture(renderer, "tilemap-image", "./assets/tilemaps/jungle.png");

    // Todo: Load the tilemap
    // We need to load that tilemap texture from ./assets/tilemaps/jungle.png
    // We need to load the file ./assets/tilemaps/jungle.map

    // Load the Tilemap
    int tileSize = 32;
    double tileScale = 2.0;
    int mapNumCols = 25;
    int mapNumRows = 20;
    std::fstream mapFile;
    mapFile.open("./assets/tilemaps/jungle.map");

    for (int y = 0; y < mapNumRows; y++)
    {
        for (int x = 0; x < mapNumCols; x++)
        {
            char ch;
            mapFile.get(ch);
            int srcRectY = std::atoi(&ch) * tileSize;
            mapFile.get(ch);
            int srcRectX = std::atoi(&ch) * tileSize;
            mapFile.ignore();

            Entity tile = registry->CreateEntity();

            tile.AddComponent<TransformComponent>(glm::vec2(x * (tileScale * tileSize), y * (tileScale * tileSize)), glm::vec2(tileScale, tileScale), 0.0);

            tile.AddComponent<SpriteComponent>("tilemap-image", tileSize, tileSize, 0, srcRectX, srcRectY);
        }
    }
    mapFile.close();

    // Create an entity and add some components to that entity
    Entity tank = registry->CreateEntity();
    tank.AddComponent<TransformComponent>(glm::vec2(10, 10), glm::vec2(1, 1), 0.0);
    tank.AddComponent<RigidbodyComponent>(glm::vec2(50, 0));
    tank.AddComponent<SpriteComponent>("tank-image", 32, 32, 2);

    Entity truck = registry->CreateEntity();
    truck.AddComponent<TransformComponent>(glm::vec2(10, 52), glm::vec2(1, 1), 0.0);
    truck.AddComponent<RigidbodyComponent>(glm::vec2(50, 0));
    truck.AddComponent<SpriteComponent>("truck-image", 32, 32, 1);
}

/// @brief Initialize all game objects
/// @details Configure all game objects before the first frame of main game loop
void Game::Setup()
{
    LoadLevel(1);
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

    // Update the registry to process the entities that are waiting to be created/deleted
    registry->Update();

    // Invoke al the systems that need to update
    registry->GetSystem<MovementSystem>().Update(deltaTime);
}

/// @brief Renders the game state
/// @details Currently empty, will be implemented with rendering logic
void Game::Render()
{
    // Working with Double-Buffered (Back and Front) Renderer
    // All of this things be render in the back buffer
    SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255);
    SDL_RenderClear(renderer);

    // Invoke all the systems that need to render
    registry->GetSystem<RenderSystem>().Update(renderer, assetStore);

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
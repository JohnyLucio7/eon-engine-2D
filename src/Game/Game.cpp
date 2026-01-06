#include "Game.h"
#include "LevelLoader.h"
#include "../Logger/Logger.h"
#include "../ECS/ECS.h"
#include "../Components/SpriteComponent.h"
#include "../Systems/MovementSystem.h"
#include "../Systems/ProjectileEmitSystem.h"
#include "../Systems/CameraMovementSystem.h"
#include "../Systems/RenderSystem.h"
#include "../Systems/AnimationSystem.h"
#include "../Systems/CollisionSystem.h"
#include "../Systems/RenderColliderSystem.h"
#include "../Systems/RenderTextSystem.h"
#include "../Systems/RenderHealthBarSystem.h"
#include "../Systems/DamageSystem.h"
#include "../Systems/KeyboardControlSystem.h"
#include "../Systems/ProjectileLifecycleSystem.h"
#include "../Systems/ScriptSystem.h"
#include <SDL2/SDL.h>
#include <fstream>
#include <string>

int Game::windowWidth;
int Game::windowHeight;
int Game::mapWidth;
int Game::mapHeight;

Game::Game() {
    isRunning = false;
    isDebug = false;
    window = nullptr;
    renderer = nullptr;
    renderWidth = 800;
    renderHeight = 600;
    registry = std::make_unique<Registry>();
    assetStore = std::make_unique<AssetStore>();
    eventBus = std::make_unique<EventBus>();
    Logger::Log("Game constructor called!");
}

Game::~Game() {
    Logger::Log("Game destructor called!");
}

void Game::AttachToWindow(void* handle, int width, int height) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        Logger::Err("[Error | Class Game | AttachToWindow] - Error initializing SDL Video Subsystem!");
        return;
    }

    window = SDL_CreateWindowFrom(handle);
    if (!window) {
        Logger::Err("[Error | Class Game | AttachToWindow] - Failed to create SDL window from handle!");
        return;
    }

    windowWidth = width;
    windowHeight = height;
    renderWidth = width;
    renderHeight = height;

    Logger::Log("\033[36m[System] Engine attached to external window handle successfully.\033[0m");
}

void Game::ResizeWindow(int width, int height) {
    windowWidth = width;
    windowHeight = height;

    // Se não estivermos usando uma resolução lógica fixa (Free Aspect), atualizamos a renderização para bater com a janela
    // Caso contrário, apenas a janela física muda, mas o RenderLogicalSize cuida da escala interna
    if (window) {
        // Em alguns sistemas SDL, o resize da janela é automático ao redimensionar o widget pai,
        // mas aqui mantemos as variáveis estáticas atualizadas.
    }
}

void Game::SetRenderLogicalSize(int width, int height) {
    renderWidth = width;
    renderHeight = height;
    if (renderer) {
        SDL_RenderSetLogicalSize(renderer, renderWidth, renderHeight);
        Logger::Log("\033[36m[System] Logical Resolution set to: " + std::to_string(width) + "x" + std::to_string(height) + "\033[0m");
    }
}

void Game::Initialize() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        Logger::Err("[Error | Class Game | Initialize] - Error initializing SDL!");
        return;
    }

    if (TTF_Init() != 0) {
        Logger::Err("[Error | Class Game | Initialize] - Error initializing TTF!");
    }

    if (!window) {
        SDL_DisplayMode displayMode;
        SDL_GetCurrentDisplayMode(0, &displayMode);
        windowWidth = displayMode.w;
        windowHeight = displayMode.h;
        renderWidth = windowWidth;
        renderHeight = windowHeight;

        window = SDL_CreateWindow(
            NULL,
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            windowWidth,
            windowHeight,
            SDL_WINDOW_BORDERLESS);
        if (!window) {
            Logger::Err("[Error | Class Game | Initialize] - Error Creating SDL window!");
            return;
        }

        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
        Logger::Log("\033[36m[System] Standalone SDL Window created successfully.\033[0m");
    }

    renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer) {
        Logger::Err("[Error | Class Game | Initialize] - Error Creating SDL renderer!");
        return;
    }

    // Aplica a resolução lógica inicial
    SDL_RenderSetLogicalSize(renderer, renderWidth, renderHeight);

    camera.x = 0;
    camera.y = 0;
    camera.w = renderWidth;
    camera.h = renderHeight;

    isRunning = true;
}

void Game::Run() {
    Setup();
    while (isRunning) {
        ProcessInput();
        Update();
        Render();
        int timeToWait = MILLISECS_PER_FRAME - (SDL_GetTicks() - millisecsPreviousFrame);
        if (timeToWait > 0 && timeToWait <= MILLISECS_PER_FRAME) {
            SDL_Delay(timeToWait);
        }
    }
}

void Game::ProcessInput() {
    SDL_Event sdlEvent;
    while (SDL_PollEvent(&sdlEvent)) {
        switch (sdlEvent.type) {
            case SDL_QUIT:
                isRunning = false;
                break;

            case SDL_KEYDOWN:
                if (sdlEvent.key.keysym.sym == SDLK_ESCAPE) {
                    isRunning = false;
                }
                if (sdlEvent.key.keysym.sym == SDLK_d) {
                    isDebug = !isDebug;
                }
                eventBus->EmitEvent<KeyPreesedEvent>(sdlEvent.key.keysym.sym);
                break;
            default:
                break;
        }
    }
}

void Game::Setup() {
    millisecsPreviousFrame = SDL_GetTicks();

    lua.open_libraries(sol::lib::base, sol::lib::math, sol::lib::os);

    LevelLoader loader;
    loader.LoadLevel(lua, registry, assetStore, renderer, 2);

    if (registry->HasSystem<ScriptSystem>()) {
        registry->GetSystem<ScriptSystem>().CreateLuaBindings(lua);
        Logger::Log("\033[32m[Info] Lua bindings created successfully.\033[0m");
    } else {
        Logger::Err("\033[31m[Error] ScriptSystem not found! Lua bindings skipped.\033[0m");
    }
}

void Game::Reload() {
    Logger::Log("\033[33m[System] Hot Reloading Scripts...\033[0m");
    registry->Clear();
    Setup();
    Logger::Log("\033[32m[System] Scripts Reloaded!\033[0m");
}

void Game::SetGameMode(GameMode mode) {
    if (gameMode == mode) return;

    if (mode == GameMode::Play) {
        Logger::Log("\033[32m[Game] Entering Play Mode. Creating Backup...\033[0m");
        if (!registryBackup) {
            registryBackup = std::make_unique<Registry>();
        }
        registryBackup->CopyStateFrom(*registry);
        gameMode = GameMode::Play;
    } else {
        Logger::Log("\033[31m[Game] Stopping Play Mode. Restoring Backup...\033[0m");
        if (registryBackup) {
            registry->CopyStateFrom(*registryBackup);
        } else {
            Logger::Err("[Game] Critical Error: No backup found to restore!");
        }
        gameMode = GameMode::Edit;
    }
}

GameMode Game::GetGameMode() const {
    return gameMode;
}

void Game::Update() {
    double deltaTime = (SDL_GetTicks() - millisecsPreviousFrame) / 1000.0;
    millisecsPreviousFrame = SDL_GetTicks();

    eventBus->Reset();

    registry->Update();

    if (gameMode == GameMode::Play) {
        registry->GetSystem<MovementSystem>().SubscribeToEvents(eventBus);
        registry->GetSystem<DamageSystem>().SubscribeToEvents(eventBus);
        registry->GetSystem<KeyboardControlSystem>().SubscribeToEvents(eventBus);
        registry->GetSystem<ProjectileEmitSystem>().SubscribeToEvents(eventBus);
        registry->GetSystem<CollisionSystem>().Update(eventBus);

        registry->GetSystem<MovementSystem>().Update(deltaTime);
        registry->GetSystem<AnimationSystem>().Update();
        registry->GetSystem<ProjectileEmitSystem>().Update(registry);
        registry->GetSystem<ProjectileLifecycleSystem>().Update();
        registry->GetSystem<ScriptSystem>().Update(deltaTime, SDL_GetTicks());
    }

    // Atualiza a câmera baseada no tamanho lógico de renderização
    camera.w = renderWidth;
    camera.h = renderHeight;
    registry->GetSystem<CameraMovementSystem>().Update(camera);
}

void Game::Render() {
    SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255);
    SDL_RenderClear(renderer);

    registry->GetSystem<RenderSystem>().Update(renderer, assetStore, camera);
    registry->GetSystem<RenderTextSystem>().Update(renderer, assetStore, camera);
    registry->GetSystem<RenderHealthBarSystem>().Update(renderer, assetStore, camera);

    if (isDebug) {
        registry->GetSystem<RenderColliderSystem>().Update(renderer, camera);
    }

    SDL_RenderPresent(renderer);
}

void Game::Destroy() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

Registry* Game::GetRegistry() const {
    return registry.get();
}

AssetStore* Game::GetAssetStore() const {
    return assetStore.get();
}

SDL_Rect& Game::GetCamera() {
    return camera;
}

SDL_Renderer* Game::GetRenderer() const {
    return renderer;
}
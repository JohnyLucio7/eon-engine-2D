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

enum class GameMode {
    Edit,
    Play
};

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
    std::unique_ptr<Registry> registryBackup;

    std::unique_ptr<AssetStore> assetStore;
    std::unique_ptr<EventBus> eventBus;

    GameMode gameMode = GameMode::Edit;

public:
    Game();
    ~Game();
    void Initialize();
    void Run();
    void Setup();
    void Reload();
    void ProcessInput();
    void Update();
    void Render();
    void Destroy();
    void AttachToWindow(void* handle, int width, int height);

    void SetGameMode(GameMode mode);
    GameMode GetGameMode() const;

    Registry* GetRegistry() const;
    AssetStore* GetAssetStore() const;
    SDL_Rect& GetCamera();
    SDL_Renderer* GetRenderer() const;

    static int windowWidth;
    static int windowHeight;
    static int mapWidth;
    static int mapHeight;
};
#endif
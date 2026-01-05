#include "./LevelLoader.h"
#include "./Game.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidbodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/AnimationComponent.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/KeyboardControlledComponent.h"
#include "../Components/CameraFollowComponent.h"
#include "../Components/ProjectileEmitterComponent.h"
#include "../Components/HealthComponent.h"
#include "../Components/ScriptComponent.h"
#include "../Components/TextLabelComponent.h"

#include "../Systems/MovementSystem.h"
#include "../Systems/RenderSystem.h"
#include "../Systems/AnimationSystem.h"
#include "../Systems/CollisionSystem.h"
#include "../Systems/RenderColliderSystem.h"
#include "../Systems/DamageSystem.h"
#include "../Systems/KeyboardControlSystem.h"
#include "../Systems/CameraMovementSystem.h"
#include "../Systems/ProjectileEmitSystem.h"
#include "../Systems/ProjectileLifecycleSystem.h"
#include "../Systems/RenderTextSystem.h"
#include "../Systems/RenderHealthBarSystem.h"
#include "../Systems/ScriptSystem.h"

#include <fstream>
#include <string>
#include <vector>
#include <functional>
#include <map>
#include <sol/sol.hpp>
#include <glm/glm.hpp>

LevelLoader::LevelLoader() {
    Logger::Log("LevelLoader constructor called!");
}

LevelLoader::~LevelLoader() {
    Logger::Log("LevelLoader destructor called!");
}

void LevelLoader::LoadLevel(sol::state &lua, std::unique_ptr<Registry> &registry,
                            const std::unique_ptr<AssetStore> &assetStore, SDL_Renderer *renderer, int levelNumber) {

    // 1. Definição da Factory de Sistemas (Movido para o topo para garantir acesso em caso de fallback)
    std::map<std::string, std::function<void()>> systemFactory = {
        {"MovementSystem", [&registry]() { registry->AddSystem<MovementSystem>(); }},
        {"RenderSystem", [&registry]() { registry->AddSystem<RenderSystem>(); }},
        {"AnimationSystem", [&registry]() { registry->AddSystem<AnimationSystem>(); }},
        {"CollisionSystem", [&registry]() { registry->AddSystem<CollisionSystem>(); }},
        {"RenderColliderSystem", [&registry]() { registry->AddSystem<RenderColliderSystem>(); }},
        {"DamageSystem", [&registry]() { registry->AddSystem<DamageSystem>(); }},
        {"KeyboardControlSystem", [&registry]() { registry->AddSystem<KeyboardControlSystem>(); }},
        {"CameraMovementSystem", [&registry]() { registry->AddSystem<CameraMovementSystem>(); }},
        {"ProjectileEmitSystem", [&registry]() { registry->AddSystem<ProjectileEmitSystem>(); }},
        {"ProjectileLifecycleSystem", [&registry]() { registry->AddSystem<ProjectileLifecycleSystem>(); }},
        {"RenderTextSystem", [&registry]() { registry->AddSystem<RenderTextSystem>(); }},
        {"RenderHealthBarSystem", [&registry]() { registry->AddSystem<RenderHealthBarSystem>(); }},
        {"ScriptSystem", [&registry]() { registry->AddSystem<ScriptSystem>(); }}
    };

    // Lista de sistemas padrão para fallback
    std::vector<std::string> defaultSystems = {
        "MovementSystem",
        "RenderSystem",
        "AnimationSystem",
        "CollisionSystem",
        "RenderColliderSystem",
        "DamageSystem",
        "KeyboardControlSystem",
        "CameraMovementSystem",
        "ProjectileEmitSystem",
        "ProjectileLifecycleSystem",
        "RenderTextSystem",
        "RenderHealthBarSystem",
        "ScriptSystem"
    };

    // 2. Tentar carregar o script Lua
    std::string scriptPath = "./assets/scripts/Level" + std::to_string(levelNumber) + ".lua";
    sol::load_result script = lua.load_file(scriptPath);

    // BLINDAGEM: Se o arquivo não existir, carregamos o básico e retornamos para evitar Crash (SIGSEGV)
    if (!script.valid()) {
        sol::error err = script;
        std::string errorMessage = err.what();
        Logger::Err("[Critical] Failed to load level script: " + errorMessage);
        Logger::Err("[Recovery] Loading default systems to prevent engine crash.");

        for (const auto& sys : defaultSystems) {
            systemFactory[sys]();
        }
        return;
    }

    // 3. Script carregado com sucesso, prosseguir com a leitura normal
    lua.script_file(scriptPath);
    sol::table level = lua["Level"];

    sol::optional<sol::table> systemsConfig = level["config"]["systems"];
    if (systemsConfig) {
        Logger::Log("\033[36m[System Config] Loading systems from Lua configuration...\033[0m");
        sol::table systemsTable = systemsConfig.value();
        for (const auto& entry : systemsTable) {
            std::string systemName = entry.second.as<std::string>();
            if (systemFactory.find(systemName) != systemFactory.end()) {
                systemFactory[systemName]();
                Logger::Log("\033[32m[System Config] Added system: " + systemName + "\033[0m");
            } else {
                Logger::Err("[System Config] System not found in factory: " + systemName);
            }
        }
    } else {
        Logger::Log("\033[33m[System Config] No config found in Lua. Loading default systems.\033[0m");
        for (const auto& sys : defaultSystems) {
            systemFactory[sys]();
        }
    }

    sol::table assets = level["assets"];
    int i = 0;
    while (true) {
        sol::optional<sol::table> hasAsset = assets[i];
        if (hasAsset == sol::nullopt) {
            break;
        }
        sol::table asset = assets[i];
        std::string assetType = asset["type"];
        std::string assetId = asset["id"];
        if (assetType == "texture") {
            assetStore->AddTexture(renderer, assetId, asset["file"]);
            Logger::Log("A new texture asset was added to the asset store, id: " + assetId);
        }
        if (assetType == "font") {
            assetStore->AddFont(assetId, asset["file"], asset["font_size"]);
            Logger::Log("A new font asset was added to the asset store, id: " + assetId);
        }
        i++;
    }

    sol::table map = level["tilemap"];
    std::string mapFilePath = map["map_file"];
    std::string mapTextureAssetId = map["texture_asset_id"];
    int mapNumRows = map["num_rows"];
    int mapNumCols = map["num_cols"];
    int tileSize = map["tile_size"];
    double mapScale = map["scale"];
    std::fstream mapFile;
    mapFile.open(mapFilePath);
    if (mapFile.is_open()) {
        for (int y = 0; y < mapNumRows; y++) {
            for (int x = 0; x < mapNumCols; x++) {
                char ch;
                mapFile.get(ch);
                int srcRectY = std::atoi(&ch) * tileSize;
                mapFile.get(ch);
                int srcRectX = std::atoi(&ch) * tileSize;
                mapFile.ignore();

                Entity tile = registry->CreateEntity();
                tile.AddComponent<TransformComponent>(glm::vec2(x * (mapScale * tileSize), y * (mapScale * tileSize)),
                                                      glm::vec2(mapScale, mapScale), 0.0);
                tile.AddComponent<SpriteComponent>(mapTextureAssetId, tileSize, tileSize, 0, false, srcRectX, srcRectY);
            }
        }
        mapFile.close();
    } else {
        Logger::Err("Failed to open map file: " + mapFilePath);
    }
    Game::mapWidth = mapNumCols * tileSize * mapScale;
    Game::mapHeight = mapNumRows * tileSize * mapScale;

    sol::table entities = level["entities"];
    i = 0;
    while (true) {
        sol::optional<sol::table> hasEntity = entities[i];
        if (hasEntity == sol::nullopt) {
            break;
        }

        sol::table entity = entities[i];

        Entity newEntity = registry->CreateEntity();

        sol::optional<std::string> tag = entity["tag"];
        if (tag != sol::nullopt) {
            newEntity.Tag(entity["tag"]);
        }

        sol::optional<std::string> group = entity["group"];
        if (group != sol::nullopt) {
            newEntity.Group(entity["group"]);
        }

        sol::optional<sol::table> hasComponents = entity["components"];
        if (hasComponents != sol::nullopt) {
            sol::optional<sol::table> transform = entity["components"]["transform"];
            if (transform != sol::nullopt) {
                newEntity.AddComponent<TransformComponent>(
                    glm::vec2(
                        entity["components"]["transform"]["position"]["x"],
                        entity["components"]["transform"]["position"]["y"]
                    ),
                    glm::vec2(
                        entity["components"]["transform"]["scale"]["x"].get_or(1.0),
                        entity["components"]["transform"]["scale"]["y"].get_or(1.0)
                    ),
                    entity["components"]["transform"]["rotation"].get_or(0.0)
                );
            }

            sol::optional<sol::table> rigidbody = entity["components"]["rigidbody"];
            if (rigidbody != sol::nullopt) {
                newEntity.AddComponent<RigidbodyComponent>(
                    glm::vec2(
                        entity["components"]["rigidbody"]["velocity"]["x"].get_or(0.0),
                        entity["components"]["rigidbody"]["velocity"]["y"].get_or(0.0)
                    )
                );
            }

            sol::optional<sol::table> sprite = entity["components"]["sprite"];
            if (sprite != sol::nullopt) {
                newEntity.AddComponent<SpriteComponent>(
                    entity["components"]["sprite"]["texture_asset_id"],
                    entity["components"]["sprite"]["width"],
                    entity["components"]["sprite"]["height"],
                    entity["components"]["sprite"]["z_index"].get_or(1),
                    entity["components"]["sprite"]["fixed"].get_or(false),
                    entity["components"]["sprite"]["src_rect_x"].get_or(0),
                    entity["components"]["sprite"]["src_rect_y"].get_or(0)
                );
            }

            sol::optional<sol::table> animation = entity["components"]["animation"];
            if (animation != sol::nullopt) {
                newEntity.AddComponent<AnimationComponent>(
                    entity["components"]["animation"]["num_frames"].get_or(1),
                    entity["components"]["animation"]["speed_rate"].get_or(1)
                );
            }

            sol::optional<sol::table> collider = entity["components"]["boxcollider"];
            if (collider != sol::nullopt) {
                newEntity.AddComponent<BoxColliderComponent>(
                    entity["components"]["boxcollider"]["width"],
                    entity["components"]["boxcollider"]["height"],
                    glm::vec2(
                        entity["components"]["boxcollider"]["offset"]["x"].get_or(0),
                        entity["components"]["boxcollider"]["offset"]["y"].get_or(0)
                    )
                );
            }

            sol::optional<sol::table> health = entity["components"]["health"];
            if (health != sol::nullopt) {
                newEntity.AddComponent<HealthComponent>(
                    static_cast<int>(entity["components"]["health"]["health_percentage"].get_or(100))
                );
            }

            sol::optional<sol::table> projectileEmitter = entity["components"]["projectile_emitter"];
            if (projectileEmitter != sol::nullopt) {
                newEntity.AddComponent<ProjectileEmitterComponent>(
                    glm::vec2(
                        entity["components"]["projectile_emitter"]["projectile_velocity"]["x"],
                        entity["components"]["projectile_emitter"]["projectile_velocity"]["y"]
                    ),
                    static_cast<int>(entity["components"]["projectile_emitter"]["repeat_frequency"].get_or(1)) * 1000,
                    static_cast<int>(entity["components"]["projectile_emitter"]["projectile_duration"].get_or(10)) * 1000,
                    static_cast<int>(entity["components"]["projectile_emitter"]["hit_percentage_damage"].get_or(10)),
                    entity["components"]["projectile_emitter"]["friendly"].get_or(false)
                );
            }

            sol::optional<sol::table> cameraFollow = entity["components"]["camera_follow"];
            if (cameraFollow != sol::nullopt) {
                newEntity.AddComponent<CameraFollowComponent>();
            }

            sol::optional<sol::table> keyboardControlled = entity["components"]["keyboard_controller"];
            if (keyboardControlled != sol::nullopt) {
                newEntity.AddComponent<KeyboardControlledComponent>(
                    glm::vec2(
                        entity["components"]["keyboard_controller"]["up_velocity"]["x"],
                        entity["components"]["keyboard_controller"]["up_velocity"]["y"]
                    ),
                    glm::vec2(
                        entity["components"]["keyboard_controller"]["right_velocity"]["x"],
                        entity["components"]["keyboard_controller"]["right_velocity"]["y"]
                    ),
                    glm::vec2(
                        entity["components"]["keyboard_controller"]["down_velocity"]["x"],
                        entity["components"]["keyboard_controller"]["down_velocity"]["y"]
                    ),
                    glm::vec2(
                        entity["components"]["keyboard_controller"]["left_velocity"]["x"],
                        entity["components"]["keyboard_controller"]["left_velocity"]["y"]
                    )
                );
            }

            sol::optional<sol::table> textLabel = entity["components"]["text_label"];
            if (textLabel != sol::nullopt) {
                newEntity.AddComponent<TextLabelComponent>(
                    glm::vec2(
                        entity["components"]["text_label"]["position"]["x"],
                        entity["components"]["text_label"]["position"]["y"]
                    ),
                    static_cast<std::string>(entity["components"]["text_label"]["text"]),
                    static_cast<std::string>(entity["components"]["text_label"]["font_id"]),
                    SDL_Color{
                        static_cast<Uint8>(entity["components"]["text_label"]["color"]["r"]),
                        static_cast<Uint8>(entity["components"]["text_label"]["color"]["g"]),
                        static_cast<Uint8>(entity["components"]["text_label"]["color"]["b"]),
                        static_cast<Uint8>(entity["components"]["text_label"]["color"]["a"])
                    },
                    static_cast<bool>(entity["components"]["text_label"]["is_fixed"])
                );
            }

            sol::optional<sol::table> script = entity["components"]["on_update_script"];
            if (script != sol::nullopt) {
                sol::function func = entity["components"]["on_update_script"][0];
                newEntity.AddComponent<ScriptComponent>(func);
            }
        }
        i++;
    }
}
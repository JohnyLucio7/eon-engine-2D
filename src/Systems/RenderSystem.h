#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/SpriteComponent.h"
#include "../AssetStore/AssetStore.h"

#include <SDL2/SDL.h>
#include <memory>
#include <algorithm>

class RenderSystem : public System {
public:
    RenderSystem() {
        RequireComponent<TransformComponent>();
        RequireComponent<SpriteComponent>();
    }

    void Update(SDL_Renderer *renderer, std::unique_ptr<AssetStore> &assetStore, SDL_Rect &camera) {
        // Todo: Sort all entities of our system by z-index

        struct RenderableEntity {
            TransformComponent transformComponent;
            SpriteComponent spriteComponent;
        };

        std::vector<RenderableEntity> renderableEntities;

        for (auto entity: GetSystemEntities()) {
            RenderableEntity renderableEntity;
            renderableEntity.transformComponent = entity.GetComponent<TransformComponent>();
            renderableEntity.spriteComponent = entity.GetComponent<SpriteComponent>();
            renderableEntities.emplace_back(renderableEntity);
        }

        std::sort(
            renderableEntities.begin(),
            renderableEntities.end(),
            [](
        const RenderableEntity &a,
        const RenderableEntity &b) {
                return a.spriteComponent.zIndex < b.spriteComponent.zIndex;
            });

        for (auto entity: renderableEntities) {
            const auto transform = entity.transformComponent;
            const auto sprite = entity.spriteComponent;

            // Set the source rectangle of our original sprite texture
            SDL_Rect srcRect = sprite.srcRect;

            // Set the destination rectangle with the x, y position to be rendered
            SDL_Rect dstRect = {
                static_cast<int>(transform.position.x - camera.x),
                static_cast<int>(transform.position.y - camera.y),
                static_cast<int>(sprite.width * transform.scale.x),
                static_cast<int>(sprite.height * transform.scale.y)
            };

            SDL_RenderCopyEx(
                renderer,
                assetStore->GetTexture(sprite.assetId),
                &srcRect,
                &dstRect,
                transform.rotation,
                NULL,
                SDL_FLIP_NONE);

            // Todo: Draw the PNG Texture
        }
    }
};

#endif /// RENDERSYSTEM_H

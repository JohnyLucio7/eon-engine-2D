//
// Created by lucio on 03/01/2026.
//

#ifndef EON_ENGINE_2D_RENDERTEXTSYSTEM_H
#define EON_ENGINE_2D_RENDERTEXTSYSTEM_H
#include "../AssetStore/AssetStore.h"
#include "../ECS/ECS.h"
#include "../Components/TextLabelComponent.h"
#include "SDL2/SDL.h"
#include <SDL2/SDL_ttf.h>

class RenderTextSystem : public System {
public:
    RenderTextSystem() {
        RequireComponent<TextLabelComponent>();
    }

    void Update(SDL_Renderer *renderer, std::unique_ptr<AssetStore> &assetStore, const SDL_Rect &camera) {
        for (auto entity: GetSystemEntities()) {
            const auto textLabelComponent = entity.GetComponent<TextLabelComponent>();

            SDL_Surface *surface = TTF_RenderText_Blended(
                assetStore->GetFont(textLabelComponent.assetId),
                textLabelComponent.text.c_str(),
                textLabelComponent.color);

            SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_FreeSurface(surface);

            int labelWidth = 0;
            int labelHeight = 0;

            SDL_QueryTexture(texture, NULL, NULL, &labelWidth, &labelHeight);

            SDL_Rect dstRect = {
                static_cast<int>(textLabelComponent.position.x - (textLabelComponent.isFixed ? 0 : camera.x)),
                static_cast<int>(textLabelComponent.position.y - (textLabelComponent.isFixed ? 0 : camera.y)),
                labelWidth,
                labelHeight
            };

            SDL_RenderCopy(renderer, texture, nullptr, &dstRect);
            SDL_DestroyTexture(texture);
        }
    }
};

#endif //EON_ENGINE_2D_RENDERTEXTSYSTEM_H

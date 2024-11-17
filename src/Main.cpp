#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <glm/glm.hpp>
#include <imgui/imgui.h>
#include <sol/sol.hpp>

int main()
{
    sol::state lua;
    lua.open_libraries(sol::lib::base);
    SDL_Init(SDL_INIT_EVERYTHING);

    glm::vec2 velocity = glm::vec2(5.0f, -2.0f);

    std::cout << "Hello, world!" << velocity.x << std::endl;
    return 0;
}

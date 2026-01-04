#include "./Game/Game.h"
#include <sol/sol.hpp>
#include <iostream>

void TestLua() {
    sol::state lua;
    lua.open_libraries(sol::lib::base);
    lua.script_file("./assets/scripts/myscript.lua");

    int someVariableInsideCpp = lua["some_variable"];
    std::cout << "The value of some_variable in C++ is: " << someVariableInsideCpp << std::endl;

    sol::table config = lua["config"];
    bool isFullscreen = config["fullscreen"];
    int width = config["resolution"]["width"];
    int height = config["resolution"]["height"];

    std::cout << "The value of isFullscreen is: " << isFullscreen << ", width: " << width << ", height: " << height <<
            std::endl;
}

int main(int argc, char *argv[]) {
    // Game game;
    //
    // game.Initialize();
    // game.Run();
    // game.Destroy();

    TestLua();

    return 0;
}

#include "./Game/Game.h"
#include <sol/sol.hpp>
#include <iostream>

int nativeCppCubeFunction(int n) {
    return (n * n * n);
}

void TestLua() {
    sol::state lua;
    lua.open_libraries(sol::lib::base);

    // This is how we expose and bind a native C++ function to be used by the Lua script
    lua["cube"] = nativeCppCubeFunction;

    lua.script_file("./assets/scripts/myscript.lua");

    int someVariableInsideCpp = lua["some_variable"];
    std::cout << "The value of some_variable in C++ is: " << someVariableInsideCpp << std::endl;

    sol::table config = lua["config"];
    bool isFullscreen = config["fullscreen"];
    int width = config["resolution"]["width"];
    int height = config["resolution"]["height"];

    std::cout << "The value of isFullscreen is: " << isFullscreen << ", width: " << width << ", height: " << height <<
            std::endl;

    // This is how we invoke a Lua function from inside C++
    sol::function functionFactorial = lua["factorial"];
    int functionResult = functionFactorial(5);
    std::cout << "The Lua script calculated 5! as " << functionResult << std::endl;
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

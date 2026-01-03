cmake_minimum_required(VERSION 3.17)
project(eon-engine-2D)

# Define o padrão C++17 conforme seu Makefile e compile_commands.json
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Inclui a pasta de bibliotecas locais (./libs/)
include_directories(libs)

# Encontra todos os arquivos .cpp dentro de src e subpastas recursivamente
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Cria o executável
add_executable(gameengine ${SOURCES}
        src/Components/CameraFollowComponent.h
        src/Systems/CameraMovementSystem.h
        src/Components/ProjectileEmitterComponent.h
        src/Systems/ProjectileEmitSystem.h
        src/Components/HealthComponent.h)

# Linka as bibliotecas que estão no seu Makefile (-lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer -llua5.3)
target_link_libraries(gameengine
        SDL2
        SDL2_image
        SDL2_ttf
        SDL2_mixer
        lua5.3
)
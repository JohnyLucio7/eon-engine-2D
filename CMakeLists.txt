cmake_minimum_required(VERSION 3.17)
project(eon-engine-2D)

# Configurações padrão do C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# -----------------------------------------------------------------------------
# Configuração do Qt (Licença LGPLv3)
# -----------------------------------------------------------------------------
# Habilita automação do Qt para Meta-Object Compiler (MOC), Resources (RCC) e UI (UIC)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Busca os componentes essenciais do Qt (Widgets para a interface desktop)
find_package(Qt5 COMPONENTS Widgets REQUIRED)

# -----------------------------------------------------------------------------
# Definição das Fontes
# -----------------------------------------------------------------------------
include_directories(libs)

# Encontra todos os arquivos .cpp recursivamente
file(GLOB_RECURSE ALL_SOURCES "src/*.cpp")

# Remove Main.cpp da lista da biblioteca (pois ele contém o main do jogo standalone)
list(FILTER ALL_SOURCES EXCLUDE REGEX ".*/src/Main.cpp$")

# Remove arquivos da pasta Editor da lista da biblioteca (eles pertencem ao executável do editor)
list(FILTER ALL_SOURCES EXCLUDE REGEX ".*/src/Editor/.*")

# -----------------------------------------------------------------------------
# Alvo 1: Biblioteca Core da Engine (Static Library)
# -----------------------------------------------------------------------------
# Contém toda a lógica: ECS, AssetStore, Game Class, Systems, Components
add_library(eon-engine-core STATIC ${ALL_SOURCES})

# Linka as dependências da Engine (SDL2, Lua, etc.)
target_link_libraries(eon-engine-core
        SDL2
        SDL2_image
        SDL2_ttf
        SDL2_mixer
        lua5.3
)

# -----------------------------------------------------------------------------
# Alvo 2: Runtime Executable (Jogo Standalone)
# -----------------------------------------------------------------------------
# Este é o executável que os jogadores receberiam (sem Qt, leve)
add_executable(eon-runtime src/Main.cpp)

# Linka com a nossa biblioteca core
target_link_libraries(eon-runtime eon-engine-core)

# -----------------------------------------------------------------------------
# Alvo 3: Editor Executable (Qt Editor)
# -----------------------------------------------------------------------------
# Encontra os arquivos específicos do Editor (que criaremos a seguir)
file(GLOB_RECURSE EDITOR_SOURCES "src/Editor/*.cpp" "src/Editor/*.h")

# Cria o executável do editor
if(EDITOR_SOURCES)
    add_executable(eon-editor ${EDITOR_SOURCES}
            src/Editor/MainWindow.h
            src/Editor/MainWindow.cpp
            src/Editor/main.cpp
            src/Editor/GameWidget.h
            src/Editor/GameWidget.cpp
            src/Editor/HierarchyPanel.h
            src/Editor/HierarchyPanel.cpp
            src/Editor/InspectorPanel.h
            src/Editor/InspectorPanel.cpp
            src/Editor/ScriptEditorPanel.h
            src/Editor/ScriptEditorPanel.cpp
            src/Editor/AssetBrowserPanel.cpp
            src/Editor/AssetBrowserPanel.h)

    # Linka com a Engine Core E com o Qt
    target_link_libraries(eon-editor
            eon-engine-core
            Qt5::Widgets
    )
endif()
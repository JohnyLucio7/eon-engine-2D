#include "Game.h"
#include <SDL2/SDL.h>
#include <iostream>

Game::Game()
{
    std::cout << "Game constructor called!" << std::endl;
}

Game::~Game()
{
    std::cout << "Game destructor called!" << std::endl;
}

void Game::Initialize()
{
    // Attempt to initialize all SDL subsystems
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        std::cerr << "[Error | Class Game | Function 'Initialize()'] - Error initializing SDL!" << std::endl;
        return;
    }

    SDL_Window *window = SDL_CreateWindow(
        NULL,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        800,
        600,
        SDL_WINDOW_BORDERLESS);

    if (!window)
    {
        std::cerr << "[Error | Class Game | Function 'Initialize()'] - Error Creating SDL window!" << std::endl;
        return;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);

    if (!renderer)
    {
        std::cerr << "[Error | Class Game | Function 'Initialize()'] - Error Creating SDL renderer!" << std::endl;
        return;
    }
}

void Game::Run()
{
    // while (true)
    // {
    //     ProcessInput();
    //     Update();
    //     Render();
    // }
}

void Game::ProcessInput() {}
void Game::Update() {}
void Game::Render() {}
void Game::Destroy() {}
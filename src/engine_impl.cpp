module;
#include <iostream>
#include "SDL.h"
module engine;

import window;
import renderer;

// 60 fps
static constexpr float s_standard_delta_time = 1.0 / 60.0;

Engine::Engine()
{
    SDL_Init(SDL_INIT_EVERYTHING);
    std::cout<<"Engine created"<<std::endl;
}

void Engine::run()
{
    Window window;
    Renderer renderer(window);
    SDL_Event event;

    tetris_.init_debug();

    float dt = s_standard_delta_time;
    uint64_t begin_time = SDL_GetTicks64();
    while(!should_close_)
    {
        renderer.render_clear_color(0, 255, 0, 1);
        
        // Handle Input
        while(SDL_PollEvent(&event)) {
            switch (event.type)
            {
                case SDL_QUIT:
                    should_close_ = true;
                    break;
                default:
                    break;
            }
        }
        
        tetris_.update_tetromino(dt);

        tetris_.render_all(renderer);

        renderer.present();

        uint64_t end_time = SDL_GetTicks64();
        dt = (end_time - begin_time) / 1000.0f;
        begin_time = end_time;

        if(dt < s_standard_delta_time)
            SDL_Delay(s_standard_delta_time - dt);
    }
    std::cout<<"Engine running"<<std::endl;
}

Engine::~Engine()
{
    std::cout<<"Engine closed"<<std::endl;
    SDL_Quit();
}
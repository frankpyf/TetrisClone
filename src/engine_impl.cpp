module;
#include <iostream>
#include "SDL.h"
module engine;

import window;
import renderer;
import tetris;
import <functional>;
import <algorithm>;


// 60 fps
static constexpr float ms_to_s = 1.f / 1000.f;

static constexpr const int32_t base_scoring_table[] = {100, 300, 500, 800};

void Game::add_score(uint8_t rows_eliminated, bool b_tspin)
{
    if(!rows_eliminated) return;
    points_ += b_tspin ? 400 * (1 + rows_eliminated) : base_scoring_table[rows_eliminated - 1] * level_;
    std::cout << "points: " << points_ << '\n';

    rows_eliminated_ += rows_eliminated;
    // level up
    if(rows_eliminated_ > level_ * 10)
    {
        std::cout << "level up\n";
        rows_eliminated_ = 0;
        ++level_;

        Tetromino* tetromino = Engine::get().get_object_by_class<Tetromino>();
        tetromino->set_update_frequency(level_);
    }
}

Engine::Engine()
{
    std::srand(time(0));
    SDL_Init(SDL_INIT_EVERYTHING);
    std::cout<<"Engine created"<<std::endl;
}

void Engine::init()
{
    Board* board = new Board(10, 20);
    Tetromino* tetromino = new Tetromino(*board);
    game_objs_.emplace_back(board);
    game_objs_.emplace_back(tetromino);
    for(auto& obj : game_objs_)
    {
        obj->init();
    }
    std::sort(game_objs_.begin(), game_objs_.end(), [](const auto& lhs, const auto& rhs){
        return lhs->render_priority_ < rhs->render_priority_;
    });
}

void Engine::run()
{
    Window window;
    Renderer renderer(window);
    SDL_Event event;

    uint64_t dt = frame_interval_;
    uint64_t begin_time = SDL_GetTicks64();
    while(!should_close_)
    {
        renderer.render_clear_color(122, 122, 122, 1);
        
        // Handle Input
        while(SDL_PollEvent(&event)) {
            switch (event.type)
            {
                case SDL_QUIT:
                    should_close_ = true;
                    break;
                case SDL_KEYDOWN:
                    input_.handle_input(event.key);
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    input_.handle_input(event.button);
                    break;
                
                default:
                    break;
            }
        }
        
        for(auto& obj : game_objs_)
        {
            obj->do_tick(dt);
        }

        for(const auto& obj : game_objs_)
        {
            obj->do_render(renderer);
        }

        renderer.present();

        uint64_t end_time = SDL_GetTicks64();
        dt = end_time - begin_time;
        begin_time = end_time;

        if(dt < frame_interval_)
            SDL_Delay(frame_interval_ - dt);
    }
}

Engine::~Engine()
{
    std::cout<<"Engine closed"<<std::endl;
    SDL_Quit();
}
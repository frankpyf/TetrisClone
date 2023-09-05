module;
#include <iostream>
#include "SDL.h"
module input;

void InputSystem::handle_input(const SDL_MouseButtonEvent& e)
{
    //std::cout<< "Handling event" <<std::endl;
}

void InputSystem::handle_input(const SDL_KeyboardEvent& e)
{
    if(callbacks_.find(e.keysym.scancode) != callbacks_.end())
        callbacks_[e.keysym.scancode]();
}

void InputSystem::bind_action(SDL_Scancode key, std::function<void()> callback_func)
{
    callbacks_.emplace(std::make_pair(key, callback_func));
}
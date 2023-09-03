module;
#include <iostream>
module input;

void InputSystem::handle_input(const SDL_MouseButtonEvent& e)
{
    std::cout<< "Handling event" <<std::endl;
    callback_();
}
void InputSystem::bind_action(SDL_Scancode key, std::function<void()> callback_func)
{
    callback_= callback_func;
}
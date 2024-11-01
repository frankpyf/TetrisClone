module;
#include "SDL.h"
#include <functional>
#include <unordered_map>
export module input;


export class InputSystem {
    std::unordered_map<SDL_Scancode, std::function<void()>> callbacks_;
public:
    void handle_input(const SDL_MouseButtonEvent& e);
    void handle_input(const SDL_KeyboardEvent& e);
    void bind_action(SDL_Scancode key, std::function<void()> callback_func);
};
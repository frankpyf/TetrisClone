module;
#include "SDL.h"
export module input;

export import <functional>;
import <map>;


export class InputSystem {
    std::unordered_map<SDL_Scancode, std::function<void()>> callbacks_;
public:
    void handle_input(const SDL_MouseButtonEvent& e);
    void handle_input(const SDL_KeyboardEvent& e);
    void bind_action(SDL_Scancode key, std::function<void()> callback_func);
};
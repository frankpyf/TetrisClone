module;
#include "SDL.h"
export module input;

export import <functional>;
import <map>;


export class InputSystem {
    std::function<void()> callback_;
public:
    void handle_input(const SDL_MouseButtonEvent& e);
    void bind_action(SDL_Scancode key, std::function<void()> callback_func);
};
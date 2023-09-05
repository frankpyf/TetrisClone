module;
#include "SDL.h"
#if !_MSC_VER
#include <functional>
#include <unordered_map>
#endif
export module input;

#if _MSC_VER && !__INTEL_COMPILER
export import <functional>;
import <unordered_map>;
#endif

export class InputSystem {
    std::unordered_map<SDL_Scancode, std::function<void()>> callbacks_;
public:
    void handle_input(const SDL_MouseButtonEvent& e);
    void handle_input(const SDL_KeyboardEvent& e);
    void bind_action(SDL_Scancode key, std::function<void()> callback_func);
};
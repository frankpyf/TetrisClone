module;
#include "SDL.h"
export module window;

import <memory>;

constexpr uint32_t SCREEN_WIDTH = 1280;
constexpr uint32_t SCREEN_HEIGHT = 720;

struct WindowDeleter {
    void operator()(SDL_Window* window)
    {
        SDL_DestroyWindow(window);
    }
};

export class Window {
    friend class Renderer;
public:
    Window(uint32_t height = SCREEN_HEIGHT, uint32_t width = SCREEN_WIDTH);
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    Window(Window&&) = default;

    ~Window() = default;

private:
    std::unique_ptr<SDL_Window, WindowDeleter> window_;

    uint32_t height_, width_;
    
};
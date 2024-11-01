module;
#include "SDL.h"
#include <memory>
export module window;

constexpr uint32_t SCREEN_WIDTH = 1280;
constexpr uint32_t SCREEN_HEIGHT = 720;

struct WindowDeleter {
    void operator()(SDL_Window* window)
    {
        SDL_DestroyWindow(window);
    }
};

export class Window {
#if _MSC_VER && !__INTEL_COMPILER
    friend class Renderer;
#else
public:
#endif   
    std::unique_ptr<SDL_Window, WindowDeleter> window_;

    uint32_t height_, width_;
public:
    Window(uint32_t height = SCREEN_HEIGHT, uint32_t width = SCREEN_WIDTH);
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    Window(Window&&) = default;

    uint32_t get_height() const { return height_; }
    uint32_t get_width() const { return width_; }

    ~Window() = default;
};
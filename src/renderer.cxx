module;
#include "SDL.h"
#if !_MSC_VER
#include <memory>
#endif
export module renderer;

import window;
#if _MSC_VER && !__INTEL_COMPILER
import <memory>;
#endif

struct RendererDeleter {
    void operator()(SDL_Renderer* renderer)
    {
        SDL_DestroyRenderer(renderer);
    }
};

export class Renderer {
public:
    Renderer(const Window&);
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    Renderer(Renderer&&) = default;

    ~Renderer() = default;

    void render_clear_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    void draw_rect_outline(const SDL_Rect& rect, const SDL_Color& color = SDL_Color{0, 0, 0, 255}) const;
    void draw_rect_fill(const SDL_Rect& rect, const SDL_Color& color = SDL_Color{0, 0, 0, 255}) const;
    void draw_circle();
    void present();
private:
    std::unique_ptr<SDL_Renderer, RendererDeleter> renderer_;
};
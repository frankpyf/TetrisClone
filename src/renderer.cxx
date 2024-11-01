module;
#include "SDL.h"
export module renderer;

import window;
import <memory>;

struct RendererDeleter {
    void operator()(SDL_Renderer* renderer)
    {
        SDL_DestroyRenderer(renderer);
    }
};

export enum class AnchorPoint : uint8_t
{
    Center = 0,
    TopLeft = 1,
    TopRight = 2,
    BottomLeft = 4,
    BottomRight = 8,
};

export class Renderer {
public:
    Renderer(Window&);
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    Renderer(Renderer&&) = default;

    ~Renderer() = default;

    void render_clear_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    void draw_rect_outline(const SDL_Rect& rect, const SDL_Color& color = SDL_Color{0, 0, 0, 255}, AnchorPoint = AnchorPoint::Center) const;
    void draw_rect_outline(const SDL_Rect& rect, int thickness = 1, const SDL_Color& color = SDL_Color{0, 0, 0, 255}, AnchorPoint = AnchorPoint::Center) const;
    void draw_rect_fill(const SDL_Rect& rect, const SDL_Color& color = SDL_Color{0, 0, 0, 255}, AnchorPoint = AnchorPoint::Center) const;
    
    void draw_circle();
    void present();
private:
    SDL_Rect translate_to_screen_coord(const SDL_Rect&, AnchorPoint) const;

    Window& window_ref_;
    std::unique_ptr<SDL_Renderer, RendererDeleter> renderer_;
};
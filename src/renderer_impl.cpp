module;
#include "SDL.h"
#if !_MSC_VER
#include <memory>
#endif
module renderer;

import window;

Renderer::Renderer(Window& in_window)
    : window_ref_(in_window)
{
    renderer_ = std::unique_ptr<SDL_Renderer, RendererDeleter>(SDL_CreateRenderer(in_window.window_.get(), -1, 0), RendererDeleter());
}

void Renderer::render_clear_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    SDL_SetRenderDrawColor(renderer_.get(), r, g, b, a);
    
    SDL_RenderClear(renderer_.get());
}

void Renderer::draw_rect_outline(const SDL_Rect &rect, const SDL_Color &color, AnchorPoint anchor) const
{
    SDL_SetRenderDrawColor(renderer_.get(), color.r, color.g, color.b, color.a);
    SDL_Rect translated_rect = translate_to_screen_coord(rect, anchor);
    SDL_RenderDrawRect(renderer_.get(), &translated_rect);
}

void Renderer::draw_rect_outline(const SDL_Rect &rect, int thickness, const SDL_Color &color, AnchorPoint anchor) const
{
    if(thickness == 0) return;
    if(thickness == 1) {draw_rect_fill(rect, color, anchor); return; }
    draw_rect_fill(rect, color, anchor);
    SDL_Rect center_rect { rect.x + thickness / 2, rect.y + thickness / 2, rect.w - thickness, rect.h - thickness};
    draw_rect_fill(center_rect, {0, 0, 0, 0}, anchor);
}

void Renderer::draw_rect_fill(const SDL_Rect& rect, const SDL_Color& color, AnchorPoint anchor) const
{
    SDL_SetRenderDrawColor(renderer_.get(), color.r, color.g, color.b, color.a);
    SDL_Rect translated_rect = translate_to_screen_coord(rect, anchor);
    SDL_RenderFillRect(renderer_.get(), &translated_rect);
}

void Renderer::present()
{
    SDL_RenderPresent(renderer_.get());
}

SDL_Rect Renderer::translate_to_screen_coord(const SDL_Rect& in_rect, AnchorPoint anchor) const
{
    uint32_t width = window_ref_.get_width();
    uint32_t height = window_ref_.get_height();
    switch (anchor)
    {
    case AnchorPoint::Center:
        return SDL_Rect(width / 2 + in_rect.x, height / 2 + in_rect.y, in_rect.w, in_rect.h);
    case AnchorPoint::TopLeft:
        return in_rect;
    case AnchorPoint::TopRight:
        return SDL_Rect(width + in_rect.x, in_rect.y, in_rect.w, in_rect.h);
    case AnchorPoint::BottomLeft:
        return SDL_Rect(in_rect.x, height + in_rect.y, in_rect.w, in_rect.h);
    case AnchorPoint::BottomRight:
        return SDL_Rect(width + in_rect.x, height + in_rect.y, in_rect.w, in_rect.h);
    }
    // log: shouldn't get here
    return in_rect;
}
 
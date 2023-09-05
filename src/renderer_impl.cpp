module;
#include "SDL.h"
#if !_MSC_VER
#include <memory>
#endif
module renderer;

import window;

Renderer::Renderer(const Window& in_window)
{
    renderer_ = std::unique_ptr<SDL_Renderer, RendererDeleter>(SDL_CreateRenderer(in_window.window_.get(), -1, 0), RendererDeleter());
}

void Renderer::render_clear_color(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    SDL_SetRenderDrawColor(renderer_.get(), r, g, b, a);
    
    SDL_RenderClear(renderer_.get());
}

void Renderer::present()
{
    SDL_RenderPresent(renderer_.get());
}

void Renderer::draw_rect_outline(const SDL_Rect& rect, const SDL_Color& color) const
{
    SDL_SetRenderDrawColor(renderer_.get(), color.r, color.g, color.b, color.a);
    SDL_RenderDrawRect(renderer_.get(), &rect);
}

void Renderer::draw_rect_fill(const SDL_Rect& rect, const SDL_Color& color) const
{
    SDL_SetRenderDrawColor(renderer_.get(), color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer_.get(), &rect);
}
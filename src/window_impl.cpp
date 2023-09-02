module;
#include "SDL.h"
module window;

Window::Window(uint32_t height, uint32_t width)
    : height_(height), width_(width)
{
    window_ = std::unique_ptr<SDL_Window, WindowDeleter>(SDL_CreateWindow(
                                                        "tetris", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                                                        width, height, SDL_WINDOW_SHOWN
                                                        ), 
                                                        WindowDeleter());
}
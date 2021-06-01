/*
 * Copyright (c) 2021 Emanuel Machado da Silva
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "sdl_game.h"

#include <iostream>

SDLGame::SDLGame(int windowWidth, int windowHeight) : isRunning{true}, window{}, renderer{}, world{} {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Could not initialize SDL2: " << SDL_GetError() << "\n";
    }

    const auto windowFlags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;
    SDL_CreateWindowAndRenderer(windowWidth, windowHeight, windowFlags, &window, &renderer);
    SDL_SetWindowTitle(window, "Bill's Ants");
}

SDLGame::~SDLGame() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void SDLGame::handleEvents() {
    static SDL_Event event;
    while (SDL_PollEvent(&event) != 0) {
        if (event.type == SDL_QUIT) {
            this->isRunning = false;
        }
    }
}

void SDLGame::draw() {
    auto texture = world.draw(renderer);
    SDL_RenderCopy(renderer, texture.get(), nullptr, nullptr);
    SDL_RenderPresent(renderer);
}

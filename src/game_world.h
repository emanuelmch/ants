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

#pragma once

#include "sdl_wrappers.h"

#include <SDL2/SDL.h>

#include <array>

const auto AntCount = 200;
const auto WorldWidth = 640;
const auto WorldHeight = 480;

struct Ant {
    SDL_Rect rect;
};

struct Colony {
    Colony() : ants{}, position{WorldWidth / 2, WorldHeight / 2} {
        const auto x = WorldWidth / 2;
        const auto y = WorldHeight / 2;
        ants.fill(Ant{x, y, 1, 1});
    }

    void run();

    std::array<Ant, AntCount> ants;
    const SDL_Point position;
};

struct GameWorld {

    GameWorld();

    inline void run() { colony.run(); }

    SDL_Texture_Wrapper draw(SDL_Renderer *renderer);

private:
    SDL_Surface_Wrapper surfaceWrapper;
    Colony colony{};
    const std::array<SDL_Rect, 13> food;
};

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

#include "game_world.h"

#include <algorithm>
#include <cassert>
#include <execution>
#include <iostream>

void Colony::run() {
    std::for_each(std::execution::par_unseq, ants.begin(), ants.end(), [](Ant &ant) {
        switch (std::rand() % 4) {
            case 0:
                ant.rect.x = std::clamp(ant.rect.x + 1, 0, WorldWidth);
                break;
            case 1:
                ant.rect.x = std::clamp(ant.rect.x - 1, 0, WorldWidth);
                break;
            case 2:
                ant.rect.y = std::clamp(ant.rect.y + 1, 0, WorldHeight);
                break;
            case 3:
                ant.rect.y = std::clamp(ant.rect.y - 1, 0, WorldHeight);
                break;
            default:
                assert(!"FUCK");
        }
    });
}

inline auto createSurface() {
    auto surface = SDL_CreateRGBSurfaceWithFormat(0, WorldWidth, WorldHeight, 32, SDL_PIXELFORMAT_RGBA32);
    return wrap(surface);
}

inline std::array<SDL_Rect, 13> createFood() {
    int centerX = WorldWidth * 3 / 4;
    int centerY = WorldHeight * 3 / 4;

    // TODO: Make this less shitty
    std::array<SDL_Rect, 13> food{};
    auto i = 0u;
    food[i] = {centerX, centerY - 2, 1, 1};
    food[++i] = {centerX - 1, centerY - 1, 1, 1};
    food[++i] = {centerX, centerY - 1, 1, 1};
    food[++i] = {centerX + 1, centerY - 1, 1, 1};
    food[++i] = {centerX - 2, centerY, 1, 1};
    food[++i] = {centerX - 1, centerY, 1, 1};
    food[++i] = {centerX, centerY, 1, 1};
    food[++i] = {centerX + 1, centerY, 1, 1};
    food[++i] = {centerX + 2, centerY, 1, 1};
    food[++i] = {centerX - 1, centerY + 1, 1, 1};
    food[++i] = {centerX, centerY+1, 1, 1};
    food[++i] = {centerX + 1, centerY + 1, 1, 1};
    food[++i] = {centerX, centerY + 2, 1, 1};

    return food;
}

GameWorld::GameWorld() : surfaceWrapper{createSurface()}, food{createFood()} {}

SDL_Texture_Wrapper GameWorld::draw(SDL_Renderer *renderer) {
    static const auto black = SDL_MapRGB(surfaceWrapper->format, 0x00, 0x00, 0x00);
    static const auto red = SDL_MapRGB(surfaceWrapper->format, 0xFF, 0x00, 0x00);
    static const auto green = SDL_MapRGB(surfaceWrapper->format, 0x00, 0xFF, 0x00);

    const auto ants = colony.ants;
    const auto surface = surfaceWrapper.get();

    SDL_FillRect(surface, nullptr, black);
    for (const auto &f: food) {
        SDL_FillRect(surface, &f, green);
    }
    for (const auto &ant: ants) {
        SDL_FillRect(surface, &ant.rect, red);
    }

    return wrap(SDL_CreateTextureFromSurface(renderer, surface));
}

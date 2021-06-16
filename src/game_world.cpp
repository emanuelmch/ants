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

#include "common.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <optional>

inline auto createSurface() {
    auto surface = SDL_CreateRGBSurfaceWithFormat(0, WorldWidth, WorldHeight, 32, SDL_PIXELFORMAT_RGBA32);
    return wrap(surface);
}

inline std::unordered_set<SDL_Rect> makeDiamond(int x, int y) {
    assert(x > 2 && x < (WorldWidth - 3));
    assert(y > 2 && y < (WorldHeight - 3));

    // TODO: Make this less shitty
    std::unordered_set<SDL_Rect> food{};
    food.emplace(x, y - 2, 1, 1);
    food.emplace(x - 1, y - 1, 1, 1);
    food.emplace(x, y - 1, 1, 1);
    food.emplace(x + 1, y - 1, 1, 1);
    food.emplace(x - 2, y, 1, 1);
    food.emplace(x - 1, y, 1, 1);
    food.emplace(x, y, 1, 1);
    food.emplace(x + 1, y, 1, 1);
    food.emplace(x + 2, y, 1, 1);
    food.emplace(x - 1, y + 1, 1, 1);
    food.emplace(x, y + 1, 1, 1);
    food.emplace(x + 1, y + 1, 1, 1);
    food.emplace(x, y + 2, 1, 1);

    return food;
}

inline std::unordered_set<SDL_Rect> makeObstacles() {
    // TODO: Make this less shitty
    std::unordered_set<SDL_Rect> obstacles{};
    obstacles.emplace(79, 70, 1, 1);
    obstacles.emplace(78, 70, 1, 1);
    obstacles.emplace(77, 70, 1, 1);
    obstacles.emplace(76, 70, 1, 1);
    obstacles.emplace(75, 70, 1, 1);
    obstacles.emplace(74, 70, 1, 1);
    obstacles.emplace(73, 70, 1, 1);
    obstacles.emplace(72, 70, 1, 1);
    obstacles.emplace(71, 70, 1, 1);
    obstacles.emplace(70, 70, 1, 1);
    obstacles.emplace(70, 71, 1, 1);
    obstacles.emplace(70, 72, 1, 1);
    obstacles.emplace(70, 73, 1, 1);
    obstacles.emplace(70, 74, 1, 1);
    obstacles.emplace(70, 75, 1, 1);
    obstacles.emplace(70, 76, 1, 1);
    obstacles.emplace(70, 77, 1, 1);
    obstacles.emplace(70, 78, 1, 1);
    obstacles.emplace(70, 79, 1, 1);
    obstacles.emplace(70, 80, 1, 1);


    return obstacles;
}

GameWorld::GameWorld() : surfaceWrapper{createSurface()},//
                         colony{WorldWidth / 2, WorldHeight / 2},//
                         food{makeDiamond(WorldWidth * 3 / 4, WorldHeight * 3 / 4)},//
                         obstacles{makeObstacles()},
                         ants{}, //
                         colonyPheromones{},//
                         foodPheromones{}//
{
    const auto x = WorldWidth / 2;
    const auto y = WorldHeight / 2;
    ants.fill(Ant{x, y});
}

inline std::optional<SDL_Point> findStrongestPheromone(const Ant &ant, const auto &pheromones) {
    auto x = static_cast<size_t>(ant.rect.x);
    auto y = static_cast<size_t>(ant.rect.y);
    auto west = (x > 0) ? pheromones[x - 1][y] : 0;
    auto east = (x < (WorldWidth - 1)) ? pheromones[x + 1][y] : 0;
    auto north = (y > 0) ? pheromones[x][y - 1] : 0;
    auto south = (y < (WorldHeight - 1)) ? pheromones[x][y + 1] : 0;

    auto strongest = common::max(west, east, north, south);
    if (strongest == 0) {
        return std::nullopt;
    }

    if (strongest == west) {
        return std::optional{SDL_Point{ant.rect.x - 1, ant.rect.y}};
    }
    if (strongest == east) {
        return std::optional{SDL_Point{ant.rect.x + 1, ant.rect.y}};
    }
    if (strongest == north) {
        return std::optional{SDL_Point{ant.rect.x, ant.rect.y - 1}};
    }
    assert(strongest == south);
    return std::optional{SDL_Point{ant.rect.x, ant.rect.y + 1}};
}

inline void moveAnt(Ant *ant, const SDL_Point &newPosition) {
    assert(newPosition.x >= 0 && newPosition.x < WorldWidth);
    assert(newPosition.y >= 0 && newPosition.y < WorldHeight);
    ant->rect.x = newPosition.x;
    ant->rect.y = newPosition.y;
}

inline void randomMove(Ant *ant, const auto &obstacles) {
    SDL_Rect move{ant->rect};
    switch (std::rand() % 4) {
        case 0:
            move.x = std::min(move.x + 1, WorldWidth - 1);
            break;
        case 1:
            move.x = std::max(move.x - 1, 0);
            break;
        case 2:
            move.y = std::min(move.y + 1, WorldHeight - 1);
            break;
        case 3:
            move.y = std::max(move.y - 1, 0);
            break;
        default:
            assert(!"Our random is broken");
    }
    if (obstacles.contains(move)) {
        randomMove(ant, obstacles);
    } else {
        ant->rect.x = move.x;
        ant->rect.y = move.y;
    }
}

inline void leavePheromone(const Ant &ant, auto *pheromones) {
    auto x = static_cast<size_t>(ant.rect.x);
    auto y = static_cast<size_t>(ant.rect.y);
    pheromones->at(x).at(y) = std::max(pheromones->at(x).at(y), ant.pheromoneStrength);
}

inline void changeAntMode(Ant *ant, const auto &colony, const auto &food) {
    if (ant->isReturning) {
        if (ant->rect.x == colony.x && ant->rect.y == colony.y) {
            ant->isReturning = false;
            ant->pheromoneStrength = PheromoneMax;
        }
    } else {
        if (food.contains(ant->rect)) {
            ant->isReturning = true;
            ant->pheromoneStrength = PheromoneMax;
        }
    }
}

void GameWorld::run() {
    for (auto &ant : ants) {
        if (rand() % 100 < RebelChance) {
            randomMove(&ant, obstacles);
        } else if (ant.isReturning) {
            leavePheromone(ant, &foodPheromones);
            auto strongestPheromone = findStrongestPheromone(ant, colonyPheromones);
            if (strongestPheromone.has_value()) {
                moveAnt(&ant, strongestPheromone.value());
            } else {
                randomMove(&ant, obstacles);
            }
        } else {
            leavePheromone(ant, &colonyPheromones);
            auto strongestPheromone = findStrongestPheromone(ant, foodPheromones);
            if (strongestPheromone.has_value()) {
                moveAnt(&ant, strongestPheromone.value());
            } else {
                randomMove(&ant, obstacles);
            }
        }

        ant.pheromoneStrength--;
        assert(ant.pheromoneStrength > 0);
        changeAntMode(&ant, colony, food);
    }
}

SDL_Texture_Wrapper GameWorld::draw(SDL_Renderer *renderer) {
    static const auto black = SDL_MapRGB(surfaceWrapper->format, 0x00, 0x00, 0x00);
    static const auto red = SDL_MapRGB(surfaceWrapper->format, 0xFF, 0x00, 0x00);
    static const auto green = SDL_MapRGB(surfaceWrapper->format, 0x00, 0xFF, 0x00);
    static const auto blue = SDL_MapRGB(surfaceWrapper->format, 0x00, 0x00, 0xFF);
    static const auto gray = SDL_MapRGB(surfaceWrapper->format, 0x64, 0x64, 0x64);

    const static SDL_Rect colonyRect{colony.x, colony.y, 1, 1};
    const auto surface = surfaceWrapper.get();

    SDL_FillRect(surface, nullptr, black);
    SDL_FillRect(surface, &colonyRect, blue);
    for (const auto &f: food) {
        SDL_FillRect(surface, &f, green);
    }
    for (const auto &o: obstacles) {
        SDL_FillRect(surface, &o, gray);
    }
    for (const auto &ant: ants) {
        SDL_FillRect(surface, &ant.rect, red);
    }

    return wrap(SDL_CreateTextureFromSurface(renderer, surface));
}

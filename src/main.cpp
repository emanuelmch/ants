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

#include <SDL2/SDL.h>

#include <chrono>
#include <iostream>

constexpr int screenWidth = 640 * 2;
constexpr int screenHeight = 480 * 2;
//constexpr int frameDelay = 1000 / 60;

int main() {
    SDLGame game{screenWidth, screenHeight};

    auto counter = 0;
    auto start = std::chrono::steady_clock::now();
    while (game.isRunning) {
//        auto start = std::chrono::steady_clock::now();

        game.handleEvents();
        game.run();
        game.draw();

        auto end = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

        counter++;
        if (duration > 1000) {
            std::cout << counter << "\n";
            counter = 0;
            start = end;
        }
//        if (duration < frameDelay) {
//            SDL_Delay(frameDelay - duration);
//        }
    }

    return 0;
}

//
// Created by joao on 20/04/2021.
//

#include "Clock.h"

Clock::Clock() { restart(); }

void Clock::restart() { start = std::chrono::high_resolution_clock::now(); }

double Clock::getElapsedTimeSeconds() const {
    Time now = std::chrono::high_resolution_clock::now();
    Duration elapsed = now - start;
    return elapsed.count();

}

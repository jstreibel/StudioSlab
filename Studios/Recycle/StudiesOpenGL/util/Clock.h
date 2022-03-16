//
// Created by joao on 20/04/2021.
//

#ifndef OPENGLSTUDIES_CLOCK_H
#define OPENGLSTUDIES_CLOCK_H

#include <chrono>

typedef std::chrono::system_clock::time_point Time;
typedef std::chrono::duration<double> Duration;

class Clock {

    Time start;
public:
    Clock();

    void restart();

    double getElapsedTimeSeconds() const;
};


#endif //OPENGLSTUDIES_CLOCK_H

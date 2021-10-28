#ifndef TIMER_H
#define TIMER_H

#include "Studios/Util/STDLibInclude.h"

class Timer
{
public:
    Timer(bool pause = true) : _isRunning(!pause), _total(0)
    {
        if(_isRunning) _start = clock();
    }
    ~Timer() {}

    void reset() { _total = 0; }
    void togglePR() {
        if(_isRunning){
             this->pause();
        }
        else{
            this->resume();
        }
    }
    void pause(){
        if(!_isRunning) return;

        _isRunning = false;
        _total += clock() - _start;
    }
    void resume(){ 
        if(_isRunning) return;

        _isRunning = true;
        _start = clock(); 
    }

    std::clock_t getTicks() {
        if(_isRunning) _updateTotalTicks();

        return _total; 
    }
    double getElTime() {

        return 0.1*(double(getTicks()) / double(CLOCKS_PER_SEC));
    }

    bool isRunning() const { return _isRunning; }

    std::clock_t getClocksPerSecond() const { return CLOCKS_PER_SEC; }

private:
    void _updateTotalTicks() {
        _total += clock() - _start;
        _start = clock();
    }

    bool _isRunning;
    std::clock_t _start, _end;
    std::clock_t _total;

};

#endif

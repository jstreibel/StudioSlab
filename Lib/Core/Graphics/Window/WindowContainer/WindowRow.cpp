//
// Created by joao on 20/08/23.
//

#include "WindowRow.h"
#include "Core/Tools/Log.h"

#include <algorithm>
#include <numeric>

#define CountLessThanZero(vec) std::count_if(begin(vec), end(vec), [](float x) { return x < 0; });
#define SumLargerThanZero(vec) std::accumulate(begin(vec), end(vec), 0.0f, [](float acc, float x) { return x > 0 ? acc + x : acc; });

#define PropagateEvent(EVENT)   \
    auto responded = false;     \
    for(auto &win : windows)    \
        if(win->doesHit(x,y)) responded = win->EVENT; \
                                \
    return responded;


void WindowRow::addWindow(Window::Ptr window, float windowWidth) {
    windows.emplace_back(window);
}

void WindowRow::arrangeWindows() {
    if(!assertConsistency()) throw "WindowRow inconsistency";

    auto m = windows.size();

    std::vector<int> computedWidths(m);
    auto freeWidths = CountLessThanZero(widths);
    if(freeWidths==m)
        for(auto &w : computedWidths) w=this->w/m;

    else if(freeWidths==0){
        for(int i=0; i<m; ++i){
            auto relWidth = this->widths[i];
            auto width = this->w * relWidth;
            computedWidths[i] = (int)width;
        }
    }

    else {
        auto reservedWidth = SumLargerThanZero(widths);
        auto wFree = (float)this->w * (1-reservedWidth) / (float)freeWidths;

        for(int i=0; i<m; ++i){
            auto relWidth = this->widths[i];
            auto width = relWidth>0 ? this->w * relWidth : wFree;
            computedWidths[i] = (int)width;
        }
    }
}

bool WindowRow::assertConsistency() const {
    auto reserverdWidth   = SumLargerThanZero(widths);
    auto freeWidths       = CountLessThanZero(widths);

    using namespace Common;

    if(freeWidths==0 && areEqual(reserverdWidth, 1))
        return true;

    if(reserverdWidth<1-(float)freeWidths*1.e-2)
        return true;

    auto &log = Log::Error() << "Inconsistent column widths: ";

    for(auto w : widths) log << (w==-1 ? Str("free") : ToStr(w)) << "; ";

    return false;
}

void WindowRow::draw() { for(auto &win : windows) win->draw(); }

void WindowRow::notifyReshape(int newWinW, int newWinH) {
    Window::notifyReshape(newWinW, newWinH);

    arrangeWindows();
}
bool WindowRow::notifyMouseMotion(int x, int y) {                           PropagateEvent(notifyMouseMotion(x,y)); }
bool WindowRow::notifyMousePassiveMotion(int x, int y){                     PropagateEvent(notifyMousePassiveMotion(x, y)); }
bool WindowRow::notifyMouseWheel(int wheel, int direction, int x, int y) {  PropagateEvent(notifyMouseWheel(wheel, direction, x, y)); }
bool WindowRow::notifyMouseButton(int button, int dir, int x, int y) {      PropagateEvent(notifyMouseButton(button, dir, x, y)); }

//
// Created by joao on 20/08/23.
//

#include "WindowColumn.h"
#include "Base/Tools/Log.h"

#include <algorithm>
#include <numeric>

#define CountLessThanZero(vec) std::count_if(vec.begin(), vec.end(), [](float x) { return x < 0; });
#define SumLargerThanZero(vec) std::accumulate(vec.begin(), vec.end(), 0.0f, [](float acc, float x) { return x > 0 ? acc + x : acc; });

#define PropagateEvent(EVENT)   \
    auto responded = false;     \
    for(auto &win : windows)    \
        if(win->doesHit(x,y)) responded = win->EVENT; \
                                \
    return responded;


void WindowColumn::addWindow(Window::Ptr window, float windowHeight) {
    windows.emplace_back(window);
    heights.emplace_back(windowHeight);
}

void WindowColumn::arrangeWindows() {
    if(!assertConsistency()) throw "WindowRow inconsistency";

    auto m = windows.size();

    std::vector<int> computedHeights(m, (int)(h/m));    // "if(freeHeights==m)"

    auto freeHeights = CountLessThanZero(heights);
    if(freeHeights==0){
        for(int i=0; i<m; ++i){
            auto relHeight = heights[i];
            auto height = h * relHeight;
            computedHeights[i] = (int)height;
        }
    } else if(freeHeights != m) {
        auto reservedHeight = SumLargerThanZero(heights);
        auto hFree = (float)h * (1-reservedHeight) / (float)freeHeights;

        for(int i=0; i<m; ++i){
            auto relHeight = this->heights[i];
            auto height = relHeight>0 ? h * relHeight : hFree;
            computedHeights[i] = (int)height;
        }
    }

    // Widths:
    std::vector<int> computedPositions(m);
    auto y = this->y;
    for(int i=0; i<m; ++i){
        computedPositions[i] = y;
        y += computedHeights[i];
    }

    auto i=0;
    for(auto &win : windows){
        win->setx(x);
        win->sety(computedPositions[i]);

        win->notifyReshape(w, computedHeights[i]);

        i++;
    }
}

bool WindowColumn::assertConsistency() const {
    auto reservedHeight   = SumLargerThanZero(heights);
    auto freeHeights      = CountLessThanZero(heights);

    using namespace Common;

    if(freeHeights==0 && areEqual(reservedHeight, 1))
        return true;

    if(reservedHeight<1-(float)freeHeights*1.e-2)
        return true;

    auto &log = Log::Error() << "Inconsistent column widths: ";

    for(auto w : heights) log << (w==-1 ? Str("free") : ToStr(w)) << "; ";

    return false;
}

void WindowColumn::draw() { for(auto &win : windows) win->draw(); }

void WindowColumn::notifyReshape(int newWinW, int newWinH) {
    Window::notifyReshape(newWinW, newWinH);

    arrangeWindows();
}
bool WindowColumn::notifyMouseMotion(int x, int y) {                           PropagateEvent(notifyMouseMotion(x,y)); }
bool WindowColumn::notifyMousePassiveMotion(int x, int y){                     PropagateEvent(notifyMousePassiveMotion(x, y)); }
bool WindowColumn::notifyMouseWheel(int wheel, int direction, int x, int y) {  PropagateEvent(notifyMouseWheel(wheel, direction, x, y)); }
bool WindowColumn::notifyMouseButton(int button, int dir, int x, int y) {      PropagateEvent(notifyMouseButton(button, dir, x, y)); }

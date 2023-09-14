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
        if(win->isMouseIn()) responded = win->EVENT; \
                                \
    return responded;


void WindowRow::addWindow(Window::Ptr window, float windowWidth) {
    windows.emplace_back(window);
    widths.emplace_back(windowWidth);
}

void WindowRow::arrangeWindows() {
    if(!assertConsistency()) throw "WindowRow inconsistency";

    auto m = windows.size();

    if(m==0) return;

    std::vector<int> computedWidths(m, (int)(getw()/m));    // "if(freeWidths==m)"

    auto freeWidths = CountLessThanZero(widths);
    if(freeWidths==0){
        for(int i=0; i<m; ++i){
            auto relWidth = widths[i];
            auto width = geth() * relWidth;
            computedWidths[i] = (int)width;
        }
    } else if(freeWidths != m) {
        auto reservedWidth = SumLargerThanZero(widths);
        auto wFree = (float)getw() * (1-reservedWidth) / (float)freeWidths;

        for(int i=0; i<m; ++i){
            auto relWidth = widths[i];
            auto width = relWidth>0 ? getw() * relWidth : wFree;
            computedWidths[i] = (int)width;
        }
    }

    std::vector<int> computed_xPositions(m);
    auto x = this->getx();
    for(int i=0; i<m; ++i){
        computed_xPositions[i] = x;
        x += computedWidths[i];
    }

    auto i=0;
    for(auto &win : windows){
        win->setx(computed_xPositions[i]);
        win->sety(gety());

        win->notifyReshape(computedWidths[i], geth());

        i++;
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

void WindowRow::notifyReshape(int w, int h) {
    Window::notifyReshape(w, h);

    arrangeWindows();
}
bool WindowRow::notifyMouseMotion(int x, int y) {                           PropagateEvent(notifyMouseMotion(x,y)); }
bool WindowRow::notifyMousePassiveMotion(int x, int y){                     PropagateEvent(notifyMousePassiveMotion(x, y)); }
bool WindowRow::notifyMouseWheel(int wheel, int direction, int x, int y) {  PropagateEvent(notifyMouseWheel(wheel, direction, x, y)); }
bool WindowRow::notifyMouseButton(int button, int dir, int x, int y) {      PropagateEvent(notifyMouseButton(button, dir, x, y)); }

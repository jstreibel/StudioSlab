//
// Created by joao on 03/09/2021.
//

#include <cassert>
#include "WindowPanel.h"
#include "Core/Graphics/OpenGL/Utils.h"
#include "Core/Tools/Log.h"


void WindowPanel::addWindow(Window *window, bool newColumn, float newColumnWidth) {
    if(newColumn){
        columns.emplace_back(WinCol());
        widths.emplace_back(newColumnWidth);

        assertConsistency();
    }

    assert(columns.size() > 0);

    auto *column = &columns.back();

    column->emplace_back(window);
}

bool WindowPanel::addWindowToColumn(Window *window, int columnId) {
    if(columns.size()-1 < columnId) return false;

    auto *column = &columns[columnId];
    column->emplace_back(window);

    return true;
}

void WindowPanel::arrangeWindows() {
    assertConsistency();

    auto m = columns.size();

    std::vector<int> computedWidths(m);
    auto freeWidths = countFreeWidths();
    if(freeWidths==m){
        for(auto &w : computedWidths) w=this->getw()/m;
    }
    else if(freeWidths==0){
        for(int i=0; i<m; ++i){
            auto relWidth = this->widths[i];
            auto width = this->getw() * relWidth;
            computedWidths[i] = width;
        }
    } else {
        auto reservedWidth = computeReservedWidth();
        auto dxFree = this->getw() * (1-reservedWidth) / freeWidths;

        for(int i=0; i<m; ++i){
            auto relWidth = this->widths[i];
            auto width = relWidth>0 ? this->getw() * relWidth : dxFree;
            computedWidths[i] = width;
        }
    }


    std::vector<int> computedPositions(m);
    auto x = this->getx();
    for(int i=0; i<m; ++i){
        computedPositions[i] = x;
        x += computedWidths[i];
    }

    auto i=0;
    for(auto &column : columns){
        auto n = column.size();
        auto dy = this->geth() / n;

        auto j=0;
        for(auto window : column){
            auto y = this->gety() + j*dy;

            window->setx(computedPositions[i]);
            window->sety(y);
            auto w = computedWidths[i];
            auto h = dy;

            window->notifyReshape(w, h);

            ++j;
        }

        ++i;
    }
}

void WindowPanel::setColumnRelativeWidth(int column, float relWidth) {
    widths[column] = relWidth;

    assertConsistency();
}

void WindowPanel::draw() {
    for(auto &column : columns)
        for(auto window : column) {
            window->draw();
            OpenGLUtils::checkGLErrors(Str(__PRETTY_FUNCTION__) + " drawing " + Common::getClassName(window));
        }
}

float WindowPanel::computeReservedWidth() const {
    auto val = .0f;
    for(auto w: widths) val += w>=0?w:0;

    return val;

}

int WindowPanel::countFreeWidths() const {
    auto val = 0;
    for(auto w: widths) if(w<=0) ++val;

    return val;
}

void WindowPanel::assertConsistency() const {

    auto resWidth = computeReservedWidth();
    auto freeWidths = countFreeWidths();

    constexpr auto epsilon = 1.e-4;

    if(freeWidths==0 && (resWidth>1-epsilon && resWidth<1+epsilon))
        return;

    if(resWidth<1-freeWidths*epsilon)
        return;

    StringStream ss;
    ss << "Inconsistent column widths: ";
    for(auto w : widths) ss << (w==-1 ? Str("free") : ToStr(w)) << "; ";


    throw ss.str();
}

bool WindowPanel::notifyMouseMotion(int x, int y) {
    auto responded = false;
    for(auto &col : columns)
        for(auto &win : col)
            if(win->isMouseIn()) responded = win->notifyMouseMotion(x, y);

    return responded;
}

void WindowPanel::notifyReshape(int newWinW, int newWinH) {
    Window::notifyReshape(newWinW, newWinH);

    arrangeWindows();
}

bool WindowPanel::notifyKeyboard(Core::KeyMap key, Core::KeyState state, Core::ModKeys modKeys) {
    return GUIEventListener::notifyKeyboard(key, state, modKeys);
}

bool WindowPanel::notifyMouseButton(Core::MouseButton button, Core::KeyState state, Core::ModKeys keys) {
    auto responded = false;
    for(auto &col : columns)
        for(auto &win : col)
            if(win->isMouseIn()) responded = win->notifyMouseButton(button, state, keys);

    return responded;
}

bool WindowPanel::notifyMouseWheel(double dx, double dy) {
    auto responded = false;
    for(auto &col : columns)
        for(auto &win : col)
            if(win->isMouseIn()) responded = win->notifyMouseWheel(dx, dy);

    return responded;
}

bool WindowPanel::notifyFilesDropped(StrVector paths) {
    auto responded = false;
    for(auto &col : columns)
        for(auto &win : col)
            if(win->isMouseIn()) responded = win->notifyFilesDropped(paths);

    return responded;
}

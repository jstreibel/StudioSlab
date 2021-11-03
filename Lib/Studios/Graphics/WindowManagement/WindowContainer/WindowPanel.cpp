//
// Created by joao on 03/09/2021.
//

#include <cassert>
#include <iostream>
#include "WindowPanel.h"



void WindowPanel::addWindow(Window *window, bool newColumn, float newColumnWidth) {
    if(newColumn){
        columns.emplace_back(WindowColumn());
        widths.emplace_back(newColumnWidth);

        assertConsistency();
    }

    auto *column = &columns.back();

    column->emplace_back(window);
}

void WindowPanel::arrangeWindows() {
    assertConsistency();


    auto m = columns.size();


    std::vector<int> computedWidths(m);
    auto freeWidths = countFreeWidths();
    if(freeWidths==m){
        for(auto &w : computedWidths) w=this->w/m;
    }
    else if(freeWidths==0){
        for(int i=0; i<m; ++i){
            auto relWidth = this->widths[i];
            auto width = this->w * relWidth;
            computedWidths[i] = width;
        }
    } else {
        auto reservedWidth = computeReservedWidth();
        auto dxFree = this->w * (1-reservedWidth) / freeWidths;

        for(int i=0; i<m; ++i){
            auto relWidth = this->widths[i];
            auto width = relWidth>0 ? this->w * relWidth : dxFree;
            computedWidths[i] = width;
        }
    }


    std::vector<int> computedPositions(m);
    auto x = this->x;
    for(int i=0; i<m; ++i){
        computedPositions[i] = x;
        x += computedWidths[i];
    }


    auto i=0;
    for(auto &column : columns){
        auto n = column.size();
        auto dy = this->h / n;

        auto j=0;
        for(auto window : column){
            auto y = this->y + j*dy;

            window->x = computedPositions[i];
            window->y = y;
            window->w = computedWidths[i];
            window->h = dy;

            ++j;
        }

        ++i;
    }
}

void WindowPanel::setColumnRelativeWidth(int column, float relWidth) {
    widths[column] = relWidth;

    assertConsistency();
}

void WindowPanel::draw(bool decorated, bool clear) const {
    for(auto &column : columns)
        for(auto window : column)
            window->draw();

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

    throw "Inconsistent column widths.";
}











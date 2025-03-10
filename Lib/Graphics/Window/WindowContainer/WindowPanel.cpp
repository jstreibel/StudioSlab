//
// Created by joao on 03/09/2021.
//

#include <cassert>
#include "WindowPanel.h"
#include "Graphics/OpenGL/Utils.h"
#include "Core/Tools/Log.h"


namespace Slab::Graphics {

    WindowPanel::WindowPanel(Config config) : SlabWindow(config) {    }

    void WindowPanel::addWindow(const Pointer<SlabWindow>& window, bool newColumn, float newColumnWidth) {
        if (newColumn) {
            columns.emplace_back();
            widths.emplace_back(newColumnWidth);

            assertConsistency();
        }

        assert(columns.size() > 0);

        auto *column = &columns.back();

        column->addWindow(window);
    }

    bool WindowPanel::removeWindow(const Pointer<SlabWindow> &windowToRemove) {
        int i=0;
        for(auto &column : columns){
            if(column.removeWindow(windowToRemove)) {
                if(column.isEmpty()){
                    columns.erase(columns.begin()+i);
                    widths.erase(widths.begin()+i);
                }

                return true;
            }

            ++i;
        }

        return false;
    }

    bool WindowPanel::addWindowToColumn(const Pointer<SlabWindow> &window, int columnId) {
        if (columns.size() - 1 < columnId) return false;

        auto *column = &columns[columnId];
        column->addWindow(window);

        return true;
    }

    void WindowPanel::arrangeWindows() {
        assertConsistency();

        auto m = columns.size();

        Vector<int> computedWidths(m);
        auto freeWidths = countFreeWidths();
        if (freeWidths == m) {
            for (auto &w: computedWidths) w = this->GetWidth() / m;
        } else if (freeWidths == 0) {
            for (int i = 0; i < m; ++i) {
                auto relWidth = this->widths[i];
                auto width = this->GetWidth() * relWidth;
                computedWidths[i] = width;
            }
        } else {
            auto reservedWidth = computeReservedWidth();
            auto dxFree = this->GetWidth() * (1 - reservedWidth) / freeWidths;

            for (int i = 0; i < m; ++i) {
                auto relWidth = this->widths[i];
                auto width = relWidth > 0 ? this->GetWidth() * relWidth : dxFree;
                computedWidths[i] = width;
            }
        }

        Vector<int> computedPositions(m);
        auto x = this->getx();
        for (int i = 0; i < m; ++i) {
            computedPositions[i] = x;
            x += computedWidths[i];
        }

        auto i = 0;
        auto y = this->gety();
        auto h = this->GetHeight();
        for (auto &column: columns) {
            column.setx(computedPositions[i]);
            column.sety(y);
            auto w = computedWidths[i];

            column.notifyReshape(w, h);

            ++i;
        }
    }

    void WindowPanel::setColumnRelativeWidth(int column, float relWidth) {
        widths[column] = relWidth;

        assertConsistency();
    }

    void WindowPanel::draw() {
        for (auto &column: columns)
            column.draw();
    }

    float WindowPanel::computeReservedWidth() const {
        auto val = .0f;
        for (auto w: widths) val += w >= 0 ? w : 0;

        return val;

    }

    int WindowPanel::countFreeWidths() const {
        auto val = 0;
        for (auto w: widths) if (w <= 0) ++val;

        return val;
    }

    void WindowPanel::assertConsistency() const {

        auto resWidth = computeReservedWidth();
        auto freeWidths = countFreeWidths();

        constexpr auto epsilon = 1.e-4;

        if (freeWidths == 0 && (resWidth > 1 - epsilon && resWidth < 1 + epsilon))
            return;

        if (resWidth < 1 - freeWidths * epsilon)
            return;

        StringStream ss;
        ss << "Inconsistent column widths: ";
        for (auto w: widths) ss << (w == -1 ? Str("free") : ToStr(w)) << "; ";


        throw ss.str();
    }

    bool WindowPanel::notifyMouseMotion(int x, int y, int dx, int dy) {
        auto responded = false;
        for (auto &col: columns) if (col.isMouseIn()) responded = col.notifyMouseMotion(x, y, dx, dy);

        return responded;
    }

    void WindowPanel::notifyReshape(int newWinW, int newWinH) {
        SlabWindow::notifyReshape(newWinW, newWinH);

        arrangeWindows();
    }

    bool
    WindowPanel::notifyKeyboard(KeyMap key, KeyState state, ModKeys modKeys) {
        // return GUIEventListener::notifyKeyboard(key, state, modKeys);

        auto responded = false;
        for (auto &col: columns)
            if (col.isMouseIn()) responded = col.notifyKeyboard(key, state, modKeys);

        return responded;
    }

    bool WindowPanel::notifyMouseButton(MouseButton button, KeyState state,
                                        ModKeys keys) {
        bool alwaysPropagate = false; // state==Core::Release;

        auto responded = false;
        for (auto &col: columns)
            if(col.isMouseIn() || alwaysPropagate) responded = col.notifyMouseButton(button, state, keys);

        return responded;
    }

    bool WindowPanel::notifyMouseWheel(double dx, double dy) {
        auto responded = false;
        for (auto &col: columns)
            if (col.isMouseIn()) responded = col.notifyMouseWheel(dx, dy);

        return responded;
    }

    void WindowPanel::setx(int x) {
        SlabWindow::setx(x);

        arrangeWindows();
    }

    void WindowPanel::sety(int y) {
        SlabWindow::sety(y);

        arrangeWindows();
    }

}
//
// Created by joao on 03/09/2021.
//

#include <cassert>
#include "WindowPanel.h"
#include "Graphics/OpenGL/Utils.h"
#include "Core/Tools/Log.h"


namespace Slab::Graphics {

    WindowPanel::WindowPanel(FSlabWindowConfig config) : FSlabWindow(config) {    }

    void WindowPanel::AddWindow(const Pointer<FSlabWindow>& window, bool newColumn, float newColumnWidth) {
        if (newColumn) {
            columns.emplace_back();
            widths.emplace_back(newColumnWidth);

            assertConsistency();
        }

        assert(columns.size() > 0);

        auto *column = &columns.back();

        column->addWindow(window);
    }

    bool WindowPanel::removeWindow(const Pointer<FSlabWindow> &windowToRemove) {
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

    bool WindowPanel::addWindowToColumn(const Pointer<FSlabWindow> &window, int columnId) {
        if (columns.size() - 1 < columnId) return false;

        auto *column = &columns[columnId];
        column->addWindow(window);

        return true;
    }

    void WindowPanel::ArrangeWindows() {
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
        auto x = this->Get_x();
        for (int i = 0; i < m; ++i) {
            computedPositions[i] = x;
            x += computedWidths[i];
        }

        auto i = 0;
        auto y = this->Get_y();
        auto h = this->GetHeight();
        for (auto &column: columns) {
            column.Set_x(computedPositions[i]);
            column.Set_y(y);
            auto w = computedWidths[i];

            column.NotifyReshape(w, h);

            ++i;
        }
    }

    void WindowPanel::SetColumnRelativeWidth(int column, float relWidth) {
        widths[column] = relWidth;

        assertConsistency();
    }

    void WindowPanel::ImmediateDraw(const FPlatformWindow& PlatformWindow) {
        for (auto &column: columns)
            column.ImmediateDraw(PlatformWindow);
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

    bool WindowPanel::NotifyMouseMotion(int x, int y, int dx, int dy) {
        auto Responded = false;
        for (auto &col: columns)
            if (col.IsPointWithin({x, y}))
                Responded = col.NotifyMouseMotion(x, y, dx, dy);

        return Responded;
    }

    void WindowPanel::NotifyReshape(int newWinW, int newWinH) {
        FSlabWindow::NotifyReshape(newWinW, newWinH);

        ArrangeWindows();
    }

    bool
    WindowPanel::NotifyKeyboard(EKeyMap key, EKeyState state, EModKeys modKeys) {
        // return GUIEventListener::notifyKeyboard(key, state, modKeys);

        auto Responded = false;
        for (auto &Col: columns)
            if (Col.IsMouseInside()) Responded = Col.NotifyKeyboard(key, state, modKeys);

        return Responded;
    }

    bool WindowPanel::NotifyMouseButton(EMouseButton button, EKeyState state,
                                        EModKeys keys) {
        constexpr bool AlwaysPropagate = false; // state==Core::Release;

        auto Responded = false;
        for (auto &Col: columns)
            if(Col.IsMouseInside() || AlwaysPropagate) Responded = Col.NotifyMouseButton(button, state, keys);

        return Responded;
    }

    bool WindowPanel::NotifyMouseWheel(double dx, double dy) {
        auto Responded = false;
        for (auto &Col: columns)
            if (Col.IsMouseInside()) Responded = Col.NotifyMouseWheel(dx, dy);

        return Responded;
    }

    void WindowPanel::Set_x(int x) {
        FSlabWindow::Set_x(x);

        ArrangeWindows();
    }

    void WindowPanel::Set_y(int y) {
        FSlabWindow::Set_y(y);

        ArrangeWindows();
    }

}
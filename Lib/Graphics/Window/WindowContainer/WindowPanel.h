//
// Created by joao on 03/09/2021.
//

#ifndef V_SHAPE_WINDOWPANEL_H
#define V_SHAPE_WINDOWPANEL_H


#include "Graphics/Window/SlabWindow.h"
#include "Graphics/Window/WindowContainer/WindowColumn.h"


namespace Slab::Graphics {

    typedef Vector<WindowColumn> WindowColumns;

    class WindowPanel : public SlabWindow {
        WindowColumns columns = WindowColumns(1);
        Vector<float> widths = {-1};


        float computeReservedWidth() const;

        int countFreeWidths() const;

        void assertConsistency() const;

    public:
        explicit WindowPanel(Config={});

        void addWindow(const Pointer<SlabWindow>& window, bool newColumn = false, float newColumnWidth = -1);

        bool removeWindow(const Pointer<SlabWindow>& window);

        /**
         * Add window to column columnId.
         * @param window the window being added.
         * @param columnId zero-based index of column.
         * @return true if success, false otherwise.
         */
        bool addWindowToColumn(const Pointer<SlabWindow> &window, int columnId);

        /**
         * Set the the relative width of a column.
         * @param column Column index, zero-based.
         * @param relWidth The relative width of the column. Negative value means a free width.
         */
        void setColumnRelativeWidth(int column, float relWidth);

        void arrangeWindows();


        void draw() override;

        void notifyReshape(int newWinW, int newWinH) override;

        bool notifyMouseMotion(int x, int y) override;

        bool notifyMouseButton(MouseButton button, KeyState state,
                               ModKeys keys) override;

        bool notifyMouseWheel(double dx, double dy) override;

        bool notifyKeyboard(KeyMap key, KeyState state, ModKeys modKeys) override;

        auto setx(int x) -> void override;

        auto sety(int y) -> void override;
    };

}

#endif //V_SHAPE_WINDOWPANEL_H

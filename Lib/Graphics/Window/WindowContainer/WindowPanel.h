//
// Created by joao on 03/09/2021.
//

#ifndef V_SHAPE_WINDOWPANEL_H
#define V_SHAPE_WINDOWPANEL_H


#include "Graphics/Window/SlabWindow.h"
#include "Graphics/Window/WindowContainer/WindowColumn.h"


namespace Slab::Graphics {

    typedef Vector<WindowColumn> WindowColumns;

    class WindowPanel : public FSlabWindow {
        WindowColumns columns = WindowColumns(1);
        Vector<float> widths = {-1};


        float computeReservedWidth() const;

        int countFreeWidths() const;

        void assertConsistency() const;

    public:
        explicit WindowPanel(Config={});

        void AddWindow(const Pointer<FSlabWindow>& window, bool newColumn = false, float newColumnWidth = -1);

        bool removeWindow(const Pointer<FSlabWindow>& window);

        /**
         * Add window to column columnId.
         * @param window the window being added.
         * @param columnId zero-based index of column.
         * @return true if success, false otherwise.
         */
        bool addWindowToColumn(const Pointer<FSlabWindow> &window, int columnId);

        /**
         * Set the the relative width of a column.
         * @param column Column index, zero-based.
         * @param relWidth The relative width of the column. Negative value means a free width.
         */
        void SetColumnRelativeWidth(int column, float relWidth);

        void arrangeWindows();


        void Draw() override;

        void NotifyReshape(int newWinW, int newWinH) override;

        bool notifyMouseMotion(int x, int y, int dx, int dy) override;

        bool NotifyMouseButton(MouseButton button, KeyState state,
                               ModKeys keys) override;

        bool notifyMouseWheel(double dx, double dy) override;

        bool NotifyKeyboard(KeyMap key, KeyState state, ModKeys modKeys) override;

        auto Set_x(int x) -> void override;

        auto Set_y(int y) -> void override;
    };

}

#endif //V_SHAPE_WINDOWPANEL_H

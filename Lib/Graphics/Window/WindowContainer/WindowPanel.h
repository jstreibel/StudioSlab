//
// Created by joao on 03/09/2021.
//

#ifndef V_SHAPE_WINDOWPANEL_H
#define V_SHAPE_WINDOWPANEL_H


#include "Graphics/Window/Window.h"
#include "Graphics/Window/WindowContainer/WindowColumn.h"


namespace Slab::Graphics {

    // typedef std::vector<Window::Ptr> WinCol;
    typedef std::vector<WindowColumn> WindowColumns;


    class WindowPanel : public Window {
        WindowColumns columns = WindowColumns(1);
        std::vector<float> widths = {-1};


        float computeReservedWidth() const;

        int countFreeWidths() const;

        void assertConsistency() const;

    public:
        explicit WindowPanel(Flags flags = None);

        void addWindow(const Window_ptr& window, bool newColumn = false, float newColumnWidth = -1);

        bool removeWindow(const Window_ptr& window);

        /**
         * Add window to column columnId.
         * @param window the window being added.
         * @param columnId zero-based index of column.
         * @return true if success, false otherwise.
         */
        bool addWindowToColumn(const Window_ptr &window, int columnId);

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

        bool notifyMouseButton(Core::MouseButton button, Core::KeyState state,
                               Core::ModKeys keys) override;

        bool notifyMouseWheel(double dx, double dy) override;

        bool notifyKeyboard(Core::KeyMap key, Core::KeyState state, Core::ModKeys modKeys) override;

        bool notifyFilesDropped(StrVector paths) override;
    };

}

#endif //V_SHAPE_WINDOWPANEL_H

//
// Created by joao on 03/09/2021.
//

#ifndef V_SHAPE_WINDOWPANEL_H
#define V_SHAPE_WINDOWPANEL_H


#include "Core/Graphics/Window/Window.h"


typedef std::vector<Window*> WinCol;
typedef std::vector<WinCol> WindowColumns;



class WindowPanel : public Window {
    WindowColumns columns = WindowColumns(1);
    std::vector<float> widths = {-1};


    float computeReservedWidth()    const;
    int countFreeWidths()           const;
    void assertConsistency()        const;

public:
    WindowPanel() = default;

    void addWindow(Window *window, bool newColumn=false, float newColumnWidth=-1);
    /**
     * Add window to column columnId.
     * @param window the window being added.
     * @param columnId zero-based index of column.
     * @return true if success, false otherwise.
     */
    bool addWindowToColumn(Window *window, int columnId);

    /**
     * Set the the relative width of a column.
     * @param column Column index, zero-based.
     * @param relWidth The relative width of the column. Negative value means a free width.
     */
    void setColumnRelativeWidth(int column, float relWidth);

    void arrangeWindows();


    void draw()       override;


    void notifyReshape(int newWinW, int newWinH) override;
    bool notifyMouseMotion(int x, int y)        override;
    bool notifyMousePassiveMotion(int x, int y) override;
    bool notifyMouseWheel(int wheel, int direction, int x, int y) override;
    bool notifyMouseButton(int button, int dir, int x, int y) override;
};


#endif //V_SHAPE_WINDOWPANEL_H
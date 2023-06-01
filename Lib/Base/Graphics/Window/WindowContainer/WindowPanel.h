//
// Created by joao on 03/09/2021.
//

#ifndef V_SHAPE_WINDOWPANEL_H
#define V_SHAPE_WINDOWPANEL_H


#include "Base/Graphics/Window/Window.h"


typedef std::vector<Window*> WindowColumn;
typedef std::vector<WindowColumn> WindowColumns;



class WindowPanel : public Window {
    WindowColumns columns = WindowColumns(1);
    std::vector<float> widths = {-1};

public:
    void addWindow(Window *window, bool newColumn=false, float newColumnWidth=-1);
    /**
     * Add window to column columnId.
     * @param window the window being added.
     * @param columnId zero-based index of column.
     * @return true if success, false otherwise.
     */
    bool addWindowToColumn(Window *window, int columnId);

    void notifyReshape(int newWinW, int newWinH) override;

    void notifyMouseMotion(int x, int y)        override;
    void notifyMousePassiveMotion(int x, int y) override;
    void draw()       override;

    void arrangeWindows();
    void setColumnRelativeWidth(int column, float relWidth);

    float computeReservedWidth()    const;
    int countFreeWidths()           const;
    void assertConsistency()        const;

};


#endif //V_SHAPE_WINDOWPANEL_H

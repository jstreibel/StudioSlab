//
// Created by joao on 03/09/2021.
//

#ifndef V_SHAPE_WINDOWPANEL_H
#define V_SHAPE_WINDOWPANEL_H


#include "Studios/Graphics/WindowManagement/Window.h"


typedef std::vector<Window*> WindowColumn;
typedef std::vector<WindowColumn> WindowColumns;



class WindowPanel : public Window {
    WindowColumns columns = WindowColumns(1);
    std::vector<float> widths = {-1};

public:
    void addWindow(Window *window, bool newColumn=false, float newColumnWidth=-1);
    void arrangeWindows();
    void setColumnRelativeWidth(int column, float relWidth);

    void draw(bool decorated, bool clear) const override;

    float computeReservedWidth() const;
    int countFreeWidths() const;
    void assertConsistency() const;
};


#endif //V_SHAPE_WINDOWPANEL_H

//
// Created by joao on 02/09/2021.
//

#ifndef V_SHAPE_NODEWINDOW_H
#define V_SHAPE_NODEWINDOW_H

#include "Base/Graphics/WindowManagement/Window.h"

#include <vector>

typedef std::vector<Window*> WindowContainer;

class WindowTreeBuilder;

class NodeWindow : public Window {
    NodeWindow(const Window &window);

    friend WindowTreeBuilder;
    enum NodeArrangement {Horizontal, Vertical};

    explicit NodeWindow(NodeWindow *parent, NodeArrangement arrangement=Horizontal);
public:
    NodeWindow(int x, int y, int w, int h);

    void addSubWindow(Window *subWindow);

    void arrange();


    void draw(bool decorated=true, bool clear=true) override;


private:
    Window *parent = nullptr;
    NodeArrangement arrangement = Horizontal;
    WindowContainer children;

};


#endif //V_SHAPE_NODEWINDOW_H

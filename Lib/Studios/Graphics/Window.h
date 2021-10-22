//
// Created by joao on 20/04/2021.
//

#ifndef V_SHAPE_WINDOW_H
#define V_SHAPE_WINDOW_H


class Window {
    int w, h, x, y;
public:
    Window(int w, int h, int x, int y);

    void bind() const;

};


#endif //V_SHAPE_WINDOW_H

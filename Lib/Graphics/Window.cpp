//
// Created by joao on 20/04/2021.
//

#include "Window.h"

#include <GL/gl.h>

Window::Window(int w, int h, int x, int y) : w(w), h(h), x(x), y(y) {}

void Window::bind() const {
    glViewport(x, y, w, h);
}

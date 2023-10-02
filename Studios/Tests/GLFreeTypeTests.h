//
// Created by joao on 27/08/23.
//

#ifndef STUDIOSLAB_GLFREETYPETESTS_H
#define STUDIOSLAB_GLFREETYPETESTS_H

#include "Graphics/OpenGL/OpenGL.h"

#include "Graphics/Window/Window.h"
#include "Graphics/OpenGL/Writer.h"

class GLFreeTypeTests : public Window {
    Graphics::Writer writer1, writer2;

public:
    GLFreeTypeTests();

    void draw() override;

    void notifyReshape(int newWinW, int newWinH) override;
};


#endif //STUDIOSLAB_GLFREETYPETESTS_H

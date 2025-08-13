//
// Created by joao on 27/08/23.
//

#ifndef STUDIOSLAB_GLFREETYPETESTS_H
#define STUDIOSLAB_GLFREETYPETESTS_H

#include "Graphics/OpenGL/OpenGL.h"

#include "Graphics/Window/SlabWindow.h"
#include "Graphics/Utils/Writer.h"
#include "Graphics/OpenGL/WriterOpenGL.h"
using Writer = Slab::Graphics::OpenGL::FWriterOpenGL;

class GLFreeTypeTests : public Slab::Graphics::FSlabWindow {
    Writer writer1, writer2;

public:
    GLFreeTypeTests();

    void ImmediateDraw(const Slab::Graphics::FPlatformWindow&) override;

    void NotifyReshape(int newWinW, int newWinH) override;
};


#endif //STUDIOSLAB_GLFREETYPETESTS_H

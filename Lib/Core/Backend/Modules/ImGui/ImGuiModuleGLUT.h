//
// Created by joao on 27/09/23.
//

#ifndef STUDIOSLAB_IMGUIMODULEGLUT_H
#define STUDIOSLAB_IMGUIMODULEGLUT_H

#include "Core/Backend/GLUT/GLUTBackend.h"
#include "Core/Backend/GLUT/GLUTListener.h"

#include "ImGuiModule.h"

namespace Slab::Core {

    class ImGuiModuleGLUT : public ImGuiModule, public GLUTListener {
    public:
        ImGuiModuleGLUT();
        ~ImGuiModuleGLUT() override;

        void beginRender() override;

        void endRender() override;

        bool keyboard(unsigned char key, int x, int y) override;
        bool keyboardUp(unsigned char key, int x, int y) override;
        bool keyboardSpecial(int key, int x, int y) override;

        bool keyboardSpecialUp(int key, int x, int y) override;

        bool mouseButton(int button, int dir, int x, int y) override;
        bool mousePassiveMotion(int x, int y) override;
        bool mouseWheel(int wheel, int dir, int x, int y) override;
        bool mouseMotion(int x, int y) override;
        bool idle() override;
        bool reshape(int w, int h) override;
    };

} // Core

#endif //STUDIOSLAB_IMGUIMODULEGLUT_H

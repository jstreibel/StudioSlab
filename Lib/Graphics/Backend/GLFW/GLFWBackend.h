//
// Created by joao on 21/09/23.
//

#ifndef STUDIOSLAB_GLFWBACKEND_H
#define STUDIOSLAB_GLFWBACKEND_H

#include "Graphics/OpenGL/OpenGL.h"
#include "glfw.h"
#include <list>

#include "Graphics/Types2D.h"

#include "Graphics/Backend/GraphicBackend.h"
#include "GLFWListener.h"
#include "GLFWEventTranslator.h"
#include "Utils/List.h"


namespace Slab::Graphics {


    class GLFWBackend : public GraphicBackend {
        void mainLoop();

        static GLFWBackend &GetInstance();

        Pointer<Slab::Graphics::SystemWindow> CreateSystemWindow(const Str& title) override;
    public:
        explicit GLFWBackend();

        ~GLFWBackend() override;

        void run() override;

        void terminate() override;

        // auto getScreenHeight() const -> Real override;
    };


}

#endif //STUDIOSLAB_GLFWBACKEND_H

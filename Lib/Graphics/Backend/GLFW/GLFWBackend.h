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


    class FGLFWBackend : public FGraphicBackend {
        void MainLoop();

        static FGLFWBackend& GetInstance();

        TPointer<Slab::Graphics::FPlatformWindow> CreatePlatformWindow(const Str& title) override;
    public:
        explicit FGLFWBackend();

        ~FGLFWBackend() override;

        void Run() override;

        void Terminate() override;

        // auto getScreenHeight() const -> DevFloat override;
    };

    using GLFWBackend [[deprecated("Use FGLFWBackend")]] = FGLFWBackend;


}

#endif //STUDIOSLAB_GLFWBACKEND_H

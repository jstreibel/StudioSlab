//
// Created by joao on 21/09/23.
//

// Thanks https://www.glfw.org/docs/3.3/input_guide.html

#include "Utils/ReferenceIterator.h"

#include "GLFWBackend.h"

#include "Core/Tools/Log.h"
#include "Core/Backend/BackendManager.h"
#include "Graphics/SlabGraphics.h"
#include "GLFWPlatformWindow.h"

namespace Slab::Graphics {

    using namespace Core;

    bool finishFlag = false;

    void errorCallback(int error_code, const char *description) {
        FLog::Error() << "GLFW error " << error_code << ": " << description << FLog::Flush;
    }

    FGLFWBackend::FGLFWBackend() : FGraphicBackend("GLFW Backend") {
        glfwSetErrorCallback(errorCallback);

        int major, minor, rev;
        glfwGetVersion(&major, &minor, &rev);

        glfwInitHint(GLFW_JOYSTICK_HAT_BUTTONS, GLFW_FALSE);

        if (!glfwInit()) throw Exception("Error initializing GLFW");
        FLog::Info() << "GLFW runtime version " << major << "." << minor << "." << rev
                       << " initialized to compile version " << GLFW_VERSION_MAJOR << "." << GLFW_VERSION_MINOR << "."
                       << GLFW_VERSION_REVISION << "." << FLog::Flush;

        if (glfwVulkanSupported()) FLog::Note() << "Vulkan is supported." << FLog::Flush;

    }

    FGLFWBackend::~FGLFWBackend() {
        SystemWindows.clear();

        glfwTerminate();

        FLog::Info() << "GLFWBackend terminated." << FLog::Flush;
    }

    void FGLFWBackend::Run() {
        MainLoop();
    }

    void FGLFWBackend::Terminate() {
        // glfwSetWindowShouldClose(systemWindow, GLFW_TRUE);
        finishFlag = true;
    }

    void FGLFWBackend::MainLoop() {

        // static auto BeginEvents = FuncRun(beginEvents);
        // static auto EndEvents   = FuncRun(endEvents);
        static auto Update   = SLAB_FUNC_RUNTHROUGH(Update);

        while(!SystemWindows.empty()) {
            IterateReferences(GraphicModules, Update);

            for(auto it = SystemWindows.begin(); it != SystemWindows.end();) {
                if((*it)->ShouldClose()) {
                    it = SystemWindows.erase(it);
                    continue;
                }

                (*it)->Render();
                ++it;
            }
        }
    }

    FGLFWBackend& FGLFWBackend::GetInstance() {
        // TODO: assert(Core::FBackendManager::GetImplementation() == Core::GLFW);

        auto guiBackend = Slab::Graphics::GetGraphicsBackend();

        return *DynamicPointerCast<FGLFWBackend>(guiBackend);
    }

    TPointer<FPlatformWindow> FGLFWBackend::CreatePlatformWindow(const Str& title) {
        TPointer<FGLFWPlatformWindow> NewPlatformWindow = New<FGLFWPlatformWindow>();
        NewPlatformWindow->ProvideSelfReference(NewPlatformWindow);

        return NewPlatformWindow;
    }
}

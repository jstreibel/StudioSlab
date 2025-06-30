//
// Created by joao on 21/09/23.
//

// Thanks https://www.glfw.org/docs/3.3/input_guide.html

#include "Utils/ReferenceIterator.h"

#include "GLFWBackend.h"

#include "Core/Tools/Log.h"
#include "Core/Backend/BackendManager.h"
#include "Graphics/SlabGraphics.h"
#include "GLFWSystemWindow.h"

namespace Slab::Graphics {

    using namespace Core;

    bool finishFlag = false;

    void errorCallback(int error_code, const char *description) {
        Log::Error() << "GLFW error " << error_code << ": " << description << Log::Flush;
    }

    GLFWBackend::GLFWBackend() : GraphicBackend("GLFW Backend") {
        glfwSetErrorCallback(errorCallback);

        int major, minor, rev;
        glfwGetVersion(&major, &minor, &rev);

        glfwInitHint(GLFW_JOYSTICK_HAT_BUTTONS, GLFW_FALSE);

        if (!glfwInit()) throw Exception("Error initializing GLFW");
        Log::Success() << "GLFW runtime version " << major << "." << minor << "." << rev
                       << " initialized to compile version " << GLFW_VERSION_MAJOR << "." << GLFW_VERSION_MINOR << "."
                       << GLFW_VERSION_REVISION << "." << Log::Flush;

        if (glfwVulkanSupported()) Log::Note() << "Vulkan is supported." << Log::Flush;

    }

    GLFWBackend::~GLFWBackend() {
        SystemWindows.clear();

        glfwTerminate();

        Log::Info() << "GLFWBackend terminated." << Log::Flush;
    }

    void GLFWBackend::Run() {
        MainLoop();
    }

    void GLFWBackend::Terminate() {
        // glfwSetWindowShouldClose(systemWindow, GLFW_TRUE);
        finishFlag = true;
    }

    void GLFWBackend::MainLoop() {

        // static auto BeginEvents = FuncRun(beginEvents);
        // static auto EndEvents   = FuncRun(endEvents);
        static auto Update   = FuncRun(Update);

        while(!SystemWindows.empty()) {
            IterateReferences(graphicModules, Update);

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

    GLFWBackend &GLFWBackend::GetInstance() {
        // TODO: assert(Core::BackendManager::GetImplementation() == Core::GLFW);

        auto guiBackend = Slab::Graphics::GetGraphicsBackend();

        return *DynamicPointerCast<GLFWBackend>(guiBackend);
    }

    Pointer<SystemWindow> GLFWBackend::CreateSystemWindow(const Str& title) {
        Pointer<GLFWSystemWindow> system_window = New<GLFWSystemWindow>();

        return system_window;
    }
}
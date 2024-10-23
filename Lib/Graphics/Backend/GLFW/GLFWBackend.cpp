//
// Created by joao on 21/09/23.
//

// Thanks https://www.glfw.org/docs/3.3/input_guide.html

#include "Utils/ReferenceIterator.h"
#include "Graphics/OpenGL/OpenGL.h"

#include "GLFWBackend.h"

#include "Core/Tools/Log.h"
#include "Core/Backend/BackendManager.h"
#include "Graphics/SlabGraphics.h"
#include "GLFWSystemWindow.h"

namespace Slab::Graphics {

    using namespace Core;

    const auto FullScreen = false;

    #define ExecFunc(funky) [](Pointer<GraphicsModule> module) {module->##funky; }

    bool finishFlag = false;

    GLFWcursor* arrowCursor;
    GLFWcursor* IBeamCursor;
    GLFWcursor* crossHairCursor;
    GLFWcursor* handCursor;
    GLFWcursor* hResizeCursor;
    GLFWcursor* vResizeCursor;

    void errorCallback(int error_code, const char *description) {
        Log::Error() << "GLFW error " << error_code << ": " << description << Log::Flush;
    }

    void window_focus_callback(GLFWwindow *window, int focused) {
        if (!focused) {
            // Here, you can handle the event when the window loses focus.
            // If you leave this empty, the window will not do anything special
            // when it loses focus, which includes not minimizing.
        }
    }


    GLFWBackend::GLFWBackend() : GraphicBackend("GLFW Backend", Naked(glfwEventTranslator)) {
        glfwSetErrorCallback(errorCallback);

        int major, minor, rev;
        glfwGetVersion(&major, &minor, &rev);

        glfwInitHint(GLFW_JOYSTICK_HAT_BUTTONS, GLFW_FALSE);

        if (!glfwInit()) throw Exception("Error initializing GLFW");
        Log::Success() << "GLFW runtime version " << major << "." << minor << "." << rev
                       << " initialized to compile version " << GLFW_VERSION_MAJOR << "." << GLFW_VERSION_MINOR << "."
                       << GLFW_VERSION_REVISION << "." << Log::Flush;

        if (glfwVulkanSupported()) Log::Note() << "Vulkan is supported." << Log::Flush;

        systemWindow = newGLFWWindow();
        glfwMakeContextCurrent(systemWindow);

        arrowCursor     = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
        IBeamCursor     = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
        crossHairCursor = glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR);
        handCursor      = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
        hResizeCursor   = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
        vResizeCursor   = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);

        static auto glfwEventTranslator_ref = Naked(glfwEventTranslator);
        addGLFWListener(glfwEventTranslator_ref);
    }

    GLFWBackend::~GLFWBackend() {
        glfwDestroyWindow(systemWindow);
        glfwTerminate();

        Log::Info() << "GLFWBackend terminated." << Log::Flush;
    }

    void GLFWBackend::run() {
        mainLoop();
    }

    void GLFWBackend::terminate() {
        // glfwSetWindowShouldClose(systemWindow, GLFW_TRUE);
        finishFlag = true;
    }

    void GLFWBackend::mainLoop() {

        while (!glfwWindowShouldClose(systemWindow) && !finishFlag) {
            // glClearColor((GLclampf)r, (GLclampf)g, (GLclampf)b, 1.f);
            glClearColor(0.25f, .25f, .35f, 1.f);
            // glClearColor(0.75f,.75f, .65f, 1.f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            auto window = systemWindow;

            static auto BeginRender = FuncRun(beginRender);
            static auto EndRender   = FuncRun(endRender);

            static auto Render      = FuncRun(Render, window);
            static auto BeginEvents = FuncRun(beginEvents);
            static auto EndEvents   = FuncRun(endEvents);

            IterateReferences(graphicModules, BeginRender);
            IterateReferences(listeners, Render);
            IterateReferences(graphicModules, EndRender);

            glfwSwapBuffers(systemWindow);

            IterateReferences(graphicModules, BeginEvents);
            glfwPollEvents();
            IterateReferences(graphicModules, EndEvents);
        }
    }

    // Real GLFWBackend::getScreenHeight() const {
    //     int w, h;
    //     glfwGetWindowSize(systemWindow, &w, &h);
    //     return h;
    // }

    GLFWwindow *GLFWBackend::newGLFWWindow() {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

        GLFWwindow *window = nullptr;

        if(FullScreen) {
            auto monitor = glfwGetPrimaryMonitor();
            const GLFWvidmode *mode = glfwGetVideoMode(monitor);
            window = glfwCreateWindow(mode->width, mode->height, "Studios Lab", monitor, nullptr);
        } else {
            fix width = 3600;
            fix height = 1800; //int(9./16. * width);

            window = glfwCreateWindow(width, height, "Studios Lab", nullptr, nullptr);
        }

        if (!window) {
            Log::Error() << "Failed creating GLFW window." << Log::Flush;
            throw Exception("GLFW error");
        }

        glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);
        glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);
        Log::Info() << "Mouse buttons and keyboard keys sticky mode " << Log::FGGreen << "ENABLED" << Log::Flush;

        fix RAW_MOUSE_MODE = false;
        if (RAW_MOUSE_MODE && glfwRawMouseMotionSupported()) {
            // When the cursor is disabled, raw (unscaled and unaccelerated) mouse motion can be enabled if available.
            // If supported, raw mouse motion can be enabled or disabled per-window and at any time but it will only be
            // provided when the cursor is disabled.
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
        } else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }

        glfwSetKeyCallback(window, key_callback);
        glfwSetCharCallback(window, window_char_callback);
        glfwSetCursorPosCallback(window, cursor_position_callback);
        glfwSetCursorEnterCallback(window, cursor_enter_callback);
        glfwSetMouseButtonCallback(window, mouse_button_callback);
        glfwSetScrollCallback(window, scroll_callback);
        glfwSetDropCallback(window, drop_callback);
        glfwSetWindowSizeCallback(window, window_size_callback);

        glfwSetWindowFocusCallback(window, window_focus_callback);
        // glfwSetWindowSize(window, );

        return window;
    }

    bool GLFWBackend::GetKeyState(GLFWwindow *window, int key) {
        fix state = glfwGetKey(window, key);

        /*
            Keys:
            GLFW_KEY_7
            GLFW_KEY_8
            GLFW_KEY_9
            GLFW_KEY_SEMICOLON
            GLFW_KEY_EQUAL
            GLFW_KEY_A
            GLFW_KEY_B
            etc.

            States:
            GLFW_PRESS or GLFW_RELEASE

            The GLFW_KEY_LAST constant holds the highest value of any named key.
         */

        return state;
    }

    Graphics::Point2D GLFWBackend::GetCursorPosition(GLFWwindow *window) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        return {xpos, ypos};
    }

    MouseState GLFWBackend::getMouseState() const {
        return mouseState;
    }

    bool GLFWBackend::IsWindowHovered(GLFWwindow *window) {
        return glfwGetWindowAttrib(window, GLFW_HOVERED);
    }

    bool GLFWBackend::GetMouseButtonState(GLFWwindow *window, int button) {
        // button: GLFW_MOUSE_BUTTON_LEFT, etc.
        // state: GLFW_PRESS or GLFW_RELEASE
        fix state = glfwGetMouseButton(window, button);
        return state;
    }

    GLFWBackend &GLFWBackend::GetInstance() {
        // assert(Core::BackendManager::GetImplementation() == Core::GLFW);

        auto &guiBackend = Slab::Graphics::GetGraphicsBackend();

        return *dynamic_cast<GLFWBackend *>(&guiBackend);
    }

    void GLFWBackend::setMouseCursor(MouseCursor cursor) {
        switch (cursor) {
            case Mouse_ArrowCursor:     glfwSetCursor(systemWindow, arrowCursor);     break;
            case Mouse_VResizeCursor:   glfwSetCursor(systemWindow, vResizeCursor);   break;
            case Mouse_HResizeCursor:   glfwSetCursor(systemWindow, hResizeCursor);   break;
            case Mouse_HandCursor:      glfwSetCursor(systemWindow, handCursor);      break;
            case Mouse_IBeamCursor:     glfwSetCursor(systemWindow, IBeamCursor);     break;
            case Mouse_CrossHairCursor: glfwSetCursor(systemWindow, crossHairCursor); break;
        }
    }

    void GLFWBackend::setSystemWindowTitle(Str title, int handle) {
        if(handle != 0) NOT_IMPLEMENTED

        glfwSetWindowTitle(systemWindow, title.c_str());
    }

    Int GLFWBackend::getSystemWindowHeight() const {
        int w, h;
        glfwGetWindowSize(systemWindow, &w, &h);

        return h;
    }


}
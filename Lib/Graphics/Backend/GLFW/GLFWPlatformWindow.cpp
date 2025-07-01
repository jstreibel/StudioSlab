//
// Created by joao on 10/22/24.
//

#include "glfw.h"

#include "GLFWPlatformWindow.h"
#include "Utils/ReferenceIterator.h"
#include "Core/Tools/Log.h"
#include "Graphics/Window/WindowStyles.h"

namespace Slab::Graphics {
    using namespace Core;

    const auto FullScreen = false;

    void FGLFWPlatformWindow::window_focus_callback(GLFWwindow *window, int focused) {
        if (!focused) {
            // Here, you can handle the event when the window loses focus.
            // If you leave this empty, the window will not do anything special
            // when it loses focus, which includes not minimizing.
        }
    }

    void FGLFWPlatformWindow::key_callback(GLFWwindow *window, int key, int scancode, int action, int mods){
        auto &me = *static_cast<FGLFWPlatformWindow*>(glfwGetWindowUserPointer(window));

        // if(key == GLFW_KEY_F11 && mods==GLFW_MOD_SHIFT && action == GLFW_PRESS)
        //     toggle_window_fullscreen(window);

        IterateReferences(me.GLFWListeners,
                          Func(KeyboardEvent, window, key, scancode, action, mods),
                          StopOnFirstResponder);
    }

    void FGLFWPlatformWindow::window_char_callback(GLFWwindow *window, unsigned int value) {
        auto &me = *static_cast<FGLFWPlatformWindow*>(glfwGetWindowUserPointer(window));

        IterateReferences(me.GLFWListeners,
                          Func(CharEvent, window, value),
                          StopOnFirstResponder);
    }

    void FGLFWPlatformWindow::cursor_position_callback(GLFWwindow *window, double xpos, double ypos) {
        auto &me = *static_cast<FGLFWPlatformWindow*>(glfwGetWindowUserPointer(window));

        IterateReferences(me.GLFWListeners,
                          Func(MouseMotion, window, xpos, ypos),
                          StopOnFirstResponder);
    }

    void FGLFWPlatformWindow::cursor_enter_callback(GLFWwindow *window, int entered) {
        auto &me = *static_cast<FGLFWPlatformWindow*>(glfwGetWindowUserPointer(window));

        IterateReferences(me.GLFWListeners,
                          FuncRun(CursorEnter, window, entered),
                          IterateAll);
    }

    void FGLFWPlatformWindow::mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
        auto &me = *static_cast<FGLFWPlatformWindow*>(glfwGetWindowUserPointer(window));

        auto iteration_policy = action==GLFW_RELEASE ? IterateAll : StopOnFirstResponder;

        IterateReferences(me.GLFWListeners,
                          Func(MouseButton, window, button, action, mods),
                          iteration_policy);
    }

    void FGLFWPlatformWindow::scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
        auto &Me = *static_cast<FGLFWPlatformWindow*>(glfwGetWindowUserPointer(window));

        IterateReferences(Me.GLFWListeners,
                          Func(MouseWheel, window, xoffset, yoffset),
                          StopOnFirstResponder);
    }

    void FGLFWPlatformWindow::drop_callback(GLFWwindow *window, int count, const char **paths) {
        StrVector pathsVec;

        auto &log = Core::Log::Info() << "Dropped files: ";
        int i;
        for (i = 0; i < count; i++) {
            log << "\n\t\t" << paths[i];
            pathsVec.emplace_back(paths[i]);
        }

        log << Core::Log::Flush;

        auto &Me = *static_cast<FGLFWPlatformWindow*>(glfwGetWindowUserPointer(window));

        IterateReferences(Me.GLFWListeners, Func(DroppedFiles, window, count, paths), IterateAll);
    }

    void FGLFWPlatformWindow::window_size_callback(GLFWwindow *window, int width, int height)
    {
        auto &Me = *static_cast<FGLFWPlatformWindow*>(glfwGetWindowUserPointer(window));

        IterateReferences(Me.GLFWListeners, FuncRun(ScreenReshape, window, width, height), IterateAll);
    }

    GLFWwindow *NewGLFWWindow() {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

        GLFWwindow *window = nullptr;

        if(FullScreen) {
            auto monitor = glfwGetPrimaryMonitor();
            const GLFWvidmode *mode = glfwGetVideoMode(monitor);
            window = glfwCreateWindow(mode->width, mode->height, "Studios Lab", monitor, nullptr);
        } else {
            Fix width = 3600;
            Fix height = 1800; //int(9./16. * width);

            window = glfwCreateWindow(width, height, "Studios Lab", nullptr, nullptr);
        }

        if (!window) {
            Log::Error() << "Failed creating GLFW window." << Log::Flush;
            throw Exception("GLFW error");
        }

        // glfwMakeContextCurrent(window);
        // glfwSwapInterval(1);

        glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);
        glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);
        Log::Info() << "Mouse buttons and keyboard keys sticky mode " << Log::FGGreen << "ENABLED" << Log::Flush;

        fix RAW_MOUSE_MODE = false;
        if (RAW_MOUSE_MODE && glfwRawMouseMotionSupported()) {
            // When the cursor is disabled, raw (unscaled and unaccelerated) mouse motion can be enabled if available.
            // If supported, raw mouse motion can be enabled or disabled per-window and at any time, but it will only be
            // provided when the cursor is disabled.
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
        } else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }

        glfwSetKeyCallback(         window, FGLFWPlatformWindow::key_callback);
        glfwSetCharCallback(        window, FGLFWPlatformWindow::window_char_callback);
        glfwSetCursorPosCallback(   window, FGLFWPlatformWindow::cursor_position_callback);
        glfwSetCursorEnterCallback( window, FGLFWPlatformWindow::cursor_enter_callback);
        glfwSetMouseButtonCallback( window, FGLFWPlatformWindow::mouse_button_callback);
        glfwSetScrollCallback(      window, FGLFWPlatformWindow::scroll_callback);
        glfwSetDropCallback(        window, FGLFWPlatformWindow::drop_callback);
        glfwSetWindowSizeCallback(  window, FGLFWPlatformWindow::window_size_callback);
        glfwSetWindowFocusCallback( window, FGLFWPlatformWindow::window_focus_callback);
        // glfwSetWindowSize(window, );

        return window;
    }

    FGLFWPlatformWindow::FGLFWPlatformWindow()
    : FPlatformWindow(NewGLFWWindow(), New<FGLFWEventTranslator>(this))
    {

        glfwSetWindowUserPointer(static_cast<GLFWwindow *>(r_Window), this);

        glfwMakeContextCurrent(static_cast<GLFWwindow *>(r_Window));

        arrowCursor     = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
        IBeamCursor     = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
        crossHairCursor = glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR);
        handCursor      = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
        hResizeCursor   = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
        vResizeCursor   = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);

        const auto Translator = DynamicPointerCast<FGLFWEventTranslator>(EventTranslator);
        AddGLFWListener(Translator);
    }

    FGLFWPlatformWindow::~FGLFWPlatformWindow() {
        glfwDestroyWindow(static_cast<GLFWwindow*>(r_Window));
    }

    Int FGLFWPlatformWindow::GetWidth() const {
        int w, h;
        glfwGetWindowSize(static_cast<GLFWwindow*>(r_Window), &w, &h);

        return w;
    }

    Int FGLFWPlatformWindow::GetHeight() const {
        int w, h;
        glfwGetWindowSize(static_cast<GLFWwindow*>(r_Window), &w, &h);

        return h;
    }

    void FGLFWPlatformWindow::AddGLFWListener(const TVolatile<Graphics::FGLFWListener>& glfwListener, const bool HighPriority) {
        if (HighPriority) GLFWListeners.emplace_front(glfwListener);
        else GLFWListeners.emplace_back(glfwListener);
    }

    void FGLFWPlatformWindow::SetMouseCursor(const FMouseCursor Cursor) {
        switch (Cursor) {
            case Mouse_ArrowCursor:     glfwSetCursor(static_cast<GLFWwindow*>(r_Window), arrowCursor);     break;
            case Mouse_VResizeCursor:   glfwSetCursor(static_cast<GLFWwindow*>(r_Window), vResizeCursor);   break;
            case Mouse_HResizeCursor:   glfwSetCursor(static_cast<GLFWwindow*>(r_Window), hResizeCursor);   break;
            case Mouse_HandCursor:      glfwSetCursor(static_cast<GLFWwindow*>(r_Window), handCursor);      break;
            case Mouse_IBeamCursor:     glfwSetCursor(static_cast<GLFWwindow*>(r_Window), IBeamCursor);     break;
            case Mouse_CrossHairCursor: glfwSetCursor(static_cast<GLFWwindow*>(r_Window), crossHairCursor); break;
        }
    }

    void FGLFWPlatformWindow::Cycle() {
        if(r_Window==nullptr) return;

        auto PlatformWindow = static_cast<GLFWwindow *>(r_Window);

        glfwMakeContextCurrent(PlatformWindow);

        glfwPollEvents();

        auto clear_color = WindowStyle::PlatformWindow_BackgroundColor;
        glClearColor(clear_color.r, clear_color.g, clear_color.b, clear_color.a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        static auto Render = FuncRun(Render, PlatformWindow);

        IterateReferences(GLFWListeners, Render, IterateAll);

        glfwSwapBuffers(static_cast<GLFWwindow *>(r_Window));
    }

    bool FGLFWPlatformWindow::ShouldClose() const {
        return glfwWindowShouldClose((GLFWwindow*)r_Window) != 0;
    }

    void FGLFWPlatformWindow::SignalClose() {
        glfwSetWindowShouldClose((GLFWwindow*)r_Window, GLFW_TRUE);
    }

    void FGLFWPlatformWindow::SetSystemWindowTitle(const Str &title) {
        auto window = (GLFWwindow*)r_Window;

        glfwSetWindowTitle(window, title.c_str());

        FPlatformWindow::SetSystemWindowTitle(title);
    }


} // Slab::Graphics
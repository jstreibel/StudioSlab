//
// Created by joao on 10/22/24.
//

#include "glfw.h"

#include "GLFWSystemWindow.h"
#include "Utils/ReferenceIterator.h"
#include "Core/Tools/Log.h"
#include "Graphics/Window/WindowStyles.h"

namespace Slab::Graphics {
    using namespace Core;

    const auto FullScreen = false;

    void GLFWSystemWindow::window_focus_callback(GLFWwindow *window, int focused) {
        if (!focused) {
            // Here, you can handle the event when the window loses focus.
            // If you leave this empty, the window will not do anything special
            // when it loses focus, which includes not minimizing.
        }
    }

    void GLFWSystemWindow::key_callback(GLFWwindow *window, int key, int scancode, int action, int mods){
        auto &me = *(GLFWSystemWindow*)glfwGetWindowUserPointer(window);

        // if(key == GLFW_KEY_F11 && mods==GLFW_MOD_SHIFT && action == GLFW_PRESS)
        //     toggle_window_fullscreen(window);

        IterateReferences(me.glfw_listeners,
                          Func(KeyboardEvent, window, key, scancode, action, mods),
                          StopOnFirstResponder);
    }

    void GLFWSystemWindow::window_char_callback(GLFWwindow *window, unsigned int value) {
        auto &me = *(GLFWSystemWindow*)glfwGetWindowUserPointer(window);

        IterateReferences(me.glfw_listeners,
                          Func(CharEvent, window, value),
                          StopOnFirstResponder);
    }

    void GLFWSystemWindow::cursor_position_callback(GLFWwindow *window, double xpos, double ypos) {
        auto &me = *(GLFWSystemWindow*)glfwGetWindowUserPointer(window);

        IterateReferences(me.glfw_listeners,
                          Func(MouseMotion, window, xpos, ypos),
                          StopOnFirstResponder);
    }

    void GLFWSystemWindow::cursor_enter_callback(GLFWwindow *window, int entered) {
        auto &me = *(GLFWSystemWindow*)glfwGetWindowUserPointer(window);

        IterateReferences(me.glfw_listeners,
                          FuncRun(CursorEnter, window, entered),
                          IterateAll);
    }

    void GLFWSystemWindow::mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
        auto &me = *(GLFWSystemWindow*)glfwGetWindowUserPointer(window);

        auto iteration_policy = action==GLFW_RELEASE ? IterateAll : StopOnFirstResponder;

        IterateReferences(me.glfw_listeners,
                          Func(MouseButton, window, button, action, mods),
                          iteration_policy);
    }

    void GLFWSystemWindow::scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
        auto &me = *(GLFWSystemWindow*)glfwGetWindowUserPointer(window);

        IterateReferences(me.glfw_listeners,
                          Func(MouseWheel, window, xoffset, yoffset),
                          StopOnFirstResponder);
    }

    void GLFWSystemWindow::drop_callback(GLFWwindow *window, int count, const char **paths) {
        StrVector pathsVec;

        auto &log = Core::Log::Info() << "Dropped files: ";
        int i;
        for (i = 0; i < count; i++) {
            log << "\n\t\t" << paths[i];
            pathsVec.emplace_back(paths[i]);
        }

        log << Core::Log::Flush;

        auto &me = *(GLFWSystemWindow*)glfwGetWindowUserPointer(window);

        IterateReferences(me.glfw_listeners, Func(DroppedFiles, window, count, paths), IterateAll);
    }

    void GLFWSystemWindow::window_size_callback(GLFWwindow *window, int width, int height) {
        auto &me = *(GLFWSystemWindow*)glfwGetWindowUserPointer(window);

        IterateReferences(me.glfw_listeners, FuncRun(ScreenReshape, window, width, height), IterateAll);
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
            fix width = 3600;
            fix height = 1800; //int(9./16. * width);

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

        glfwSetKeyCallback(         window, GLFWSystemWindow::key_callback);
        glfwSetCharCallback(        window, GLFWSystemWindow::window_char_callback);
        glfwSetCursorPosCallback(   window, GLFWSystemWindow::cursor_position_callback);
        glfwSetCursorEnterCallback( window, GLFWSystemWindow::cursor_enter_callback);
        glfwSetMouseButtonCallback( window, GLFWSystemWindow::mouse_button_callback);
        glfwSetScrollCallback(      window, GLFWSystemWindow::scroll_callback);
        glfwSetDropCallback(        window, GLFWSystemWindow::drop_callback);
        glfwSetWindowSizeCallback(  window, GLFWSystemWindow::window_size_callback);
        glfwSetWindowFocusCallback( window, GLFWSystemWindow::window_focus_callback);
        // glfwSetWindowSize(window, );

        return window;
    }

    GLFWSystemWindow::GLFWSystemWindow()
    : SystemWindow(NewGLFWWindow(), New<GLFWEventTranslator>())
    {

        glfwSetWindowUserPointer(static_cast<GLFWwindow *>(window_ptr), this);

        glfwMakeContextCurrent(static_cast<GLFWwindow *>(window_ptr));

        arrowCursor     = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
        IBeamCursor     = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
        crossHairCursor = glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR);
        handCursor      = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
        hResizeCursor   = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
        vResizeCursor   = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);

        addGLFWListener(DynamicPointerCast<GLFWEventTranslator>(event_translator));
    }

    GLFWSystemWindow::~GLFWSystemWindow() {
        glfwDestroyWindow((GLFWwindow*)window_ptr);
    }

    Int GLFWSystemWindow::getWidth() const {
        int w, h;
        glfwGetWindowSize((GLFWwindow*)window_ptr, &w, &h);

        return w;
    }

    Int GLFWSystemWindow::getHeight() const {
        int w, h;
        glfwGetWindowSize((GLFWwindow*)window_ptr, &w, &h);

        return h;
    }

    void GLFWSystemWindow::addGLFWListener(const Volatile<Graphics::GLFWListener>& glfwListener, bool highPriority) {
        if (highPriority) glfw_listeners.emplace_front(glfwListener);
        else glfw_listeners.emplace_back(glfwListener);
    }

    void GLFWSystemWindow::setMouseCursor(MouseCursor cursor) {
        switch (cursor) {
            case Mouse_ArrowCursor:     glfwSetCursor((GLFWwindow*)window_ptr, arrowCursor);     break;
            case Mouse_VResizeCursor:   glfwSetCursor((GLFWwindow*)window_ptr, vResizeCursor);   break;
            case Mouse_HResizeCursor:   glfwSetCursor((GLFWwindow*)window_ptr, hResizeCursor);   break;
            case Mouse_HandCursor:      glfwSetCursor((GLFWwindow*)window_ptr, handCursor);      break;
            case Mouse_IBeamCursor:     glfwSetCursor((GLFWwindow*)window_ptr, IBeamCursor);     break;
            case Mouse_CrossHairCursor: glfwSetCursor((GLFWwindow*)window_ptr, crossHairCursor); break;
        }
    }

    void GLFWSystemWindow::Cycle() {
        if(window_ptr==nullptr) return;

        glfwMakeContextCurrent((GLFWwindow*)window_ptr);

        glfwPollEvents();

        auto clear_color = WindowStyle::sysWindowBGColor;
        glClearColor(clear_color.r, clear_color.g, clear_color.b, clear_color.a);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        auto window = static_cast<GLFWwindow *>(window_ptr);
        static auto Render = FuncRun(Render, window);

        IterateReferences(glfw_listeners, Render);

        glfwSwapBuffers(static_cast<GLFWwindow *>(window_ptr));
    }

    bool GLFWSystemWindow::ShouldClose() const {
        return glfwWindowShouldClose((GLFWwindow*)window_ptr) != 0;
    }

    void GLFWSystemWindow::SignalClose() {
        glfwSetWindowShouldClose((GLFWwindow*)window_ptr, GLFW_TRUE);
    }

    void GLFWSystemWindow::setSystemWindowTitle(const Str &title) {
        auto window = (GLFWwindow*)window_ptr;

        glfwSetWindowTitle(window, title.c_str());

        SystemWindow::setSystemWindowTitle(title);
    }


} // Slab::Graphics
//
// Created by joao on 21/09/23.
//

// Thanks https://www.glfw.org/docs/3.3/input_guide.html

#include "Graphics/OpenGL/OpenGL.h"

#include <GLFW/glfw3.h>
#include "GLFWBackend.h"

#include "Core/Tools/Log.h"
#include "Core/Backend/BackendManager.h"

bool finishFlag = false;

void errorCallback(int error_code, const char* description){
    Log::Error() << "GLFW error " << error_code << ": " << description << Log::Flush;
}

GLFWBackend::GLFWBackend() : GraphicBackend("GLFW Backend", eventTranslator) {
    glfwSetErrorCallback(errorCallback);

    int major, minor, rev;
    glfwGetVersion(&major, &minor, &rev);

    glfwInitHint(GLFW_JOYSTICK_HAT_BUTTONS, GLFW_FALSE);

    if (!glfwInit()) throw Str("Error initializing GLFW");
    Log::Success() << "GLFW runtime version " << major << "." << minor << "." << rev
                   << " initialized to compile version " << GLFW_VERSION_MAJOR << "." << GLFW_VERSION_MINOR << "." << GLFW_VERSION_REVISION << "." << Log::Flush;

    if(glfwVulkanSupported()) Log::Note() << "Vulkan is supported." << Log::Flush;

    systemWindow = newGLFWWindow();
    glfwMakeContextCurrent(systemWindow);

    addGLFWListener(&eventTranslator);
}

GLFWBackend::~GLFWBackend() {
    glfwDestroyWindow(systemWindow);
    glfwTerminate();

    Log::Info() << "GLFWBackend terminated." << Log::Flush;
}

void GLFWBackend::run(Program *pProgram) {
    this->program = pProgram;

    mainLoop();
}

void GLFWBackend::finish() {
    // glfwSetWindowShouldClose(systemWindow, GLFW_TRUE);
    finishFlag = true;
}

void GLFWBackend::mainLoop() {
    while (!glfwWindowShouldClose(systemWindow) && !finishFlag)
    {
        while(!mustRender && !paused && program->cycle(Program::CycleOptions::CycleUntilOutput));
        mustRender = false;

        glClearColor(r, g, b, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        for(auto &module : modules) module->beginRender();
        for(auto &listener : GetInstance().listeners) listener->Render(systemWindow);
        for(auto &module : modules) module->endRender();

        glfwSwapBuffers(systemWindow);

        for(auto &module : modules) module->beginEvents();
        glfwPollEvents();
        for(auto &module : modules) module->endEvents();
    }
}

Real GLFWBackend::getScreenHeight() const {
    int w, h;
    glfwGetWindowSize(systemWindow, &w, &h);

    return h;
}

void GLFWBackend::pause() { paused = true; }

void GLFWBackend::resume() { paused = false; }

void GLFWBackend::requestRender() { mustRender = true; }

GLFWwindow *GLFWBackend::newGLFWWindow() {
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

    auto monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "Studios Lab", monitor, nullptr);
    if (!window){
        Log::Error() << "Failed creating GLFW window." << Log::Flush;
        throw "GLFW error";
    }

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);
    glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, GLFW_TRUE);
    Log::Info() << "Mouse buttons and keyboard keys sticky mode " << Log::FGGreen << "ENABLED" << Log::Flush;

    fix RAW_MOUSE_MODE = false;
    if(RAW_MOUSE_MODE && glfwRawMouseMotionSupported()){
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
    assert(Core::BackendManager::GetImplementation() == Core::GLFW);

    auto &guiBackend = Core::BackendManager::GetGUIBackend();

    return *dynamic_cast<GLFWBackend*>(&guiBackend);
}





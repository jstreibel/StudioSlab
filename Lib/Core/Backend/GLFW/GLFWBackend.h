//
// Created by joao on 21/09/23.
//

#ifndef STUDIOSLAB_GLFWBACKEND_H
#define STUDIOSLAB_GLFWBACKEND_H

#include <GLFW/glfw3.h>
#include "../GUIBackend.h"

class GLFWBackend : public GUIBackend {
    Program* program = nullptr;
    GLFWwindow *systemWindow = nullptr;

    void initGLEW();

    void mainLoop();

    static GLFWwindow *newGLFWWindow();

    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
    static void cursor_enter_callback(GLFWwindow* window, int entered);
    static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
    static void drop_callback(GLFWwindow* window, int count, const char** paths);
    static void window_size_callback(GLFWwindow* window, int width, int height);

    bool paused = false;
    bool mustRender = true;

    static GLFWBackend& GetInstance();

public:
    explicit GLFWBackend();

    ~GLFWBackend() override;

    static bool GetKeyState(GLFWwindow *window, int key);
    static Point2D GetCursorPosition(GLFWwindow *window);
    static bool IsWindowHovered(GLFWwindow *window);
    static bool GetMouseButtonState(GLFWwindow *window, int button);

    auto getMouseState() const -> const MouseState override;

    auto getGLFWWindow() -> GLFWwindow&;

    void run(Program *program) override;

    auto getScreenHeight() const -> Real override;

    auto pause() -> void override;
    auto resume() -> void override;

    auto requestRender() -> void override;
};


#endif //STUDIOSLAB_GLFWBACKEND_H

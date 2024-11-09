//
// Created by joao on 21/09/23.
//

#include "glfw.h"

namespace Slab::Graphics {

    struct WindowedModeConfig {
        int x=10, y=10;
        int width=3400, height=1700;
    } windowed_mode_config;

    bool isWindowFullscreen(GLFWwindow* window) {
        return glfwGetWindowMonitor(window) != nullptr;

    }

    void setWindowFullscreen(GLFWwindow* window) {
        // Get the primary monitor
        GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
        if (!primaryMonitor) {
            // Handle error: no monitor found
            return;
        }

        // Get the video mode of the monitor
        const GLFWvidmode* videoMode = glfwGetVideoMode(primaryMonitor);
        if (!videoMode) {
            // Handle error: no video mode found
            return;
        }

        // Store current windowed mode size and position
        glfwGetWindowPos(window, &windowed_mode_config.x, &windowed_mode_config.y);
        glfwGetWindowSize(window, &windowed_mode_config.width, &windowed_mode_config.height);

        // Set the window to fullscreen
        glfwSetWindowMonitor(window, primaryMonitor, 0, 0, videoMode->width, videoMode->height, videoMode->refreshRate);
    }

    void setWindowWindowed(GLFWwindow* window) {
        // Set the window back to windowed mode
        glfwSetWindowMonitor(window, nullptr,
                             windowed_mode_config.x, windowed_mode_config.y,
                             windowed_mode_config.width, windowed_mode_config.height, 0);
    }

    void toggle_window_fullscreen(GLFWwindow *window) {
        if (isWindowFullscreen(window)) {
            setWindowWindowed(window);
        } else {
            setWindowFullscreen(window);
        }
    }


}
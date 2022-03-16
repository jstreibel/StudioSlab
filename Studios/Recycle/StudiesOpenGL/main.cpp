#include <nanogui/screen.h>
#include <nanogui/window.h>


#include "Studios/OpenGL/StudiesOpenGL/Studies/PendulumStudy.h"
#include "Studios/OpenGL/StudiesOpenGL/Studies/FieldStudy/FieldStudy.h"
#include "Studios/OpenGL/StudiesOpenGL/Studies/ChaosGame/LexyStudy.h"

#include "Studios/OpenGL/StudiesOpenGL/OpenGLUtils/Utils.h"
#include "Studios/OpenGL/StudiesOpenGL/Studies/OgLplusStudy.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

int MAIN_WINDOW_WIDTH = 1000,
    MAIN_WINDOW_HEIGHT = 1000;

nanogui::Screen *screen = nullptr;
//GLFWwindow* window = nullptr;
StudiesBase *currentStudy = nullptr;

bool initGraphics();
bool initStudies();
void mainLoop();
void finishGraphics();

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

void GLAPIENTRY
OpenGLMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam );

// ******************************** //
// ************* MAIN ************* //
// **                            ** //
// **                            ** //
int main() {
    if(!initGraphics()) return -1;

    if(!initStudies()) return -2;

    try { mainLoop(); }
    catch (const char *e) {
        std::cout << "\n\nException reached the top of main: " << "\033[91m\033[1m"
                  << e << "\033[0m" << ", application will now exit" << std::endl;
        return -1;
    } catch (std::string &e) {
        std::cout << "\n\nException reached the top of main: " << "\033[91m\033[1m"
                  << e << "\033[0m" << ", application will now exit" << std::endl;
        return -1;
    }

    finishGraphics();

    return 0;
}

bool initGraphics() {
    nanogui::init();

    screen = new nanogui::Screen({MAIN_WINDOW_WIDTH, MAIN_WINDOW_HEIGHT}, "Studies");
    screen->setVisible(true);
    //window->setLayout();

    if(!glfwInit()) {
        std::cout << "Error: 0x" << std::hex << glfwGetError(NULL) << std::endl;
        return false;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    glewExperimental = true;
    glewInit();

    glEnable              ( GL_DEBUG_OUTPUT );
    glDebugMessageCallback(OpenGLMessageCallback, 0);

    printf("OpenGL version %s\n", glGetString(GL_VERSION));

    return true;
}

bool initStudies(){ 
    try {
        const int study = 3;
        switch (study) {
            case 1: currentStudy = new PendulumStudy; break;
            case 2: currentStudy = new FieldStudy(MAIN_WINDOW_WIDTH, MAIN_WINDOW_HEIGHT); break;
            case 3: currentStudy = new LexyStudy(*screen); break;
            case 4: currentStudy = new OGLplusStudy; break;
        }
    } catch (const char *e) {
        std::cout << "\n\nException reached the top of main: " << "\033[91m\033[1m"
                  << e << "\033[0m" << ", application will now exit" << std::endl;
        return -1;
    } catch (std::string &e) {
        std::cout << "\n\nException reached the top of main: " << "\033[91m\033[1m"
                  << e << "\033[0m" << ", application will now exit" << std::endl;
        return -1;
    }

    return true;
}

void mainLoop() {

    ulong fullLoopsCounter = 0;
    while(!glfwWindowShouldClose(screen->glfwWindow())){
        if (glfwGetKey(screen->glfwWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(screen->glfwWindow(), GL_TRUE);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        currentStudy->doYourThing();

        screen->drawWidgets();

        glfwSwapBuffers(screen->glfwWindow());
        glfwPollEvents();

        fullLoopsCounter++;
    }
}

void finishGraphics() {
    nanogui::shutdown();
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if(key == GLFW_KEY_ESCAPE) glfwSetWindowShouldClose(window, GL_TRUE);
    //if (key == GLFW_KEY_E && action == GLFW_PRESS)
    //    activate_airship();
    //if (key == GLFW_KEY_E && action == GLFW_PRESS)
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    //if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    //    popup_menu();
}

#define BOOST_STACKTRACE_USE_ADDR2LINE
#include <boost/stacktrace.hpp>
void GLAPIENTRY
OpenGLMessageCallback(GLenum source,
                      GLenum type,
                      GLuint id,
                      GLenum severity,
                      GLsizei length,
                      const GLchar* message,
                      const void* userParam )
{
    if(1 && severity == GL_DEBUG_SEVERITY_NOTIFICATION) return;

    std::string sevStr = "";
    FILE *file = nullptr;


    switch (severity){
        case GL_DEBUG_SEVERITY_LOW: sevStr = "LOW"; break;
        case GL_DEBUG_SEVERITY_MEDIUM: sevStr = "MEDIUM"; break;
        case GL_DEBUG_SEVERITY_HIGH: sevStr = "HIGH"; break;
        case GL_DEBUG_SEVERITY_NOTIFICATION: sevStr = "NOTIFICATION"; break;
        default: sevStr = "UNKNOWN";
    }

    if(type == GL_DEBUG_TYPE_ERROR) file = stderr;
    else file = stdout;

    fprintf( file, "OpenGL callback %s\n\tSeverity = %s (0x%x), type = 0x%x\n\tmessage = \"%s\"\n", ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ), sevStr.c_str(), severity, type, message );
    fflush(file);
    if(0) std::cout << boost::stacktrace::stacktrace() << std::endl;

    std::cout << "";
}
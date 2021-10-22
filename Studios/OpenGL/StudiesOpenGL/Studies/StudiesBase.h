//
// Created by joao on 08/04/2021.
//

#ifndef OPENGLTUTORIAL_STUDIESBASE_H
#define OPENGLTUTORIAL_STUDIESBASE_H

#include <GLFW/glfw3.h>



class StudiesBase {
public:
    virtual void doYourThing() = 0;
    virtual void keyEvent(GLFWwindow& window, int key, int scancode, int action, int mods) { };
    virtual void mouseEvent() { };
    virtual void resize(GLuint newWindowWidth, GLuint newWindowHeight) { };
};


#endif //OPENGLTUTORIAL_STUDIESBASE_H

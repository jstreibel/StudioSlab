//
// Created by joao on 07/04/2021.
//

#include "PendulumStudy.h"
#include "Studios/OpenGL/StudiesOpenGL/OpenGLUtils/Utils.h"

#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>

PendulumStudy::PendulumStudy() {
    std::string fileLoc = "/home/joao/Developer/OpenGLStudies/shade/";
    shader.loadShaders(fileLoc + "pendulumPhaseSpace.vert",
                       fileLoc + "pendulumPhaseSpace.frag");

    resize(800, 600);
}

void PendulumStudy::doYourThing() {
    glViewport(0, 0, winWidth, winHeight);
    shader.bind();

    canvas.draw();
}

void PendulumStudy::resize(GLuint newWindowWidth, GLuint newWindowHeight){
    winWidth = newWindowWidth;
    winHeight = newWindowHeight;

    const float halfQuadSize = 0.8;

    std::vector<GLfloat> quad(12);
    quad[0] = -halfQuadSize; quad[1]  = -halfQuadSize; quad[2] = 0;
    quad[3] = -halfQuadSize; quad[4]  =  halfQuadSize; quad[5] = 0;
    quad[6] =  halfQuadSize; quad[7]  =  halfQuadSize; quad[8] = 0;
    quad[9] =  halfQuadSize; quad[10] = -halfQuadSize; quad[11] = 0;

    shader.setInt("width", (int)newWindowWidth);
    shader.setInt("height", (int)newWindowHeight);
}

void PendulumStudy::event(sf::Event &event) {
    if(event.type == sf::Event::Resized)
        resize(event.size.width, event.size.height);

}


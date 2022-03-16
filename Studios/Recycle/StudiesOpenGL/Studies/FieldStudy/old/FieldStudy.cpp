//
// Created by joao on 08/04/2021.
//

#include <cmath>
#include "FieldStudy.h"
#include "Studios/OpenGL/StudiesOpenGL/OpenGLUtils/Utils.h"


FieldStudy_old::FieldStudy_old(int winWidth, int winHeight) : posSpaceVP(0, 0, winWidth, winHeight),
                           fourierSpaceVP(0, 0, 200, 200){
    std::string fileLoc = "/home/joao/Developer/OpenGLStudies/shade/";

    lineShader.loadShaders(fileLoc + "lines.vert",
                           fileLoc + "lines.frag");

    resize(winWidth, winHeight);

    const int dim = 1;
    const int N = 200;
    std::vector<GLfloat> lineData((N+1) * dim);
    const float dt = 1. / N;
    for (int i = 0; i <= N; i++) {
        const float t = -1 + 2 * i * dt;

        const float x = t;
        lineData[i] = x; // X
    }
    line.setData(lineData, GL_LINE_STRIP, dim);

    clock.restart();

    //spectrumTexture.loadFromFile("/home/joao/Developer/OpenGLStudies/Studies/FieldStudy/spectra/spectra.png");
    //spectrumSprite = sf::Sprite(spectrumTexture);
    //spectrumSprite.setPosition(0, 0);
}

void FieldStudy_old::doYourThing(GLFWwindow &window) {

    if(clock.getElapsedTimeSeconds() > 0.0016){
        phase += 0.025;
        lineShader.setFloat("phase", phase);

        clock.restart();
    }

    posSpaceVP.set();
    lineShader.bind();
    const float dl = 0.056;
    lineShader.setInt("vert", false);
    for(float yPos=-1; yPos<=1; yPos += dl) {
        lineShader.setFloat("yPos", yPos);
        line.draw();
    }

    lineShader.setInt("vert", true);
    for(float yPos=-1; yPos<=1; yPos += dl) {
        lineShader.setFloat("yPos", yPos);
        line.draw();
    }

    fourierSpaceVP.set();

    lineShader.setInt("vert", false);
    for(float yPos=-1; yPos<=1; yPos += dl) {
        lineShader.setFloat("yPos", yPos);
        line.draw();
    }

    lineShader.setInt("vert", true);
    for(float yPos=-1; yPos<=1; yPos += dl) {
        lineShader.setFloat("yPos", yPos);
        line.draw();
    }

}

/*
void FieldStudy::event(sf::Event &event) {
    if(event.type == sf::Event::Resized)
        resize(event.size.width, event.size.height);
    else if(event.type == sf::Event::MouseMoved){
        sf::Vector2i mouseNew = {event.mouseMove.x, event.mouseMove.y};
        mouseDelta = mouseNew - mousePos;
        mousePos = mouseNew;

        if(sf::Mouse::isButtonPressed(sf::Mouse::Left)){
            const float k = 0.01;
            xRot += mouseDelta.y*k;
            yRot += mouseDelta.x*k;

            lineShader.setFloat("xRot", xRot);
            lineShader.setFloat("yRot", yRot);
        }
    }
    else if(event.type == sf::Event::MouseButtonPressed) {

    }
}

void FieldStudy::resize(GLuint newWindowWidth, GLuint newWindowHeight) {
    winWidth = newWindowWidth;
    winHeight = newWindowHeight;

    fourierSpaceVP.x = winWidth - fourierSpaceVP.w;
    fourierSpaceVP.y = winHeight - fourierSpaceVP.h;
}

*/
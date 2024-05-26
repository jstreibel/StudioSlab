//
// Created by joao on 08/04/2021.
//

#include <cmath>
#include "FieldStudy.h"
#include "Studios/OpenGL/StudiesOpenGL/OpenGLUtils/Utils.h"
#include "Studios/OpenGL/StudiesOpenGL/util/util.h"
#include <oglplus/vertex_attrib.hpp>
#include <oglplus/uniform.hpp>
#include <oglplus/context.hpp>
#include "Studios/OpenGL/StudiesOpenGL/OpenGLUtils/Utils.h"

FieldStudy::FieldStudy(int winWidth, int winHeight) /*: posSpaceVP(0, 0, winWidth, winHeight),
                           fourierSpaceVP(0, 0, 200, 200) */ {
    std::string fileLoc = "/home/joao/Developer/OpenGLStudies/shade/";

    oglplus::VertexShader vertShader(oglplus::GLSLString(loadFileToString(fileLoc + "lines.vert")));
    oglplus::FragmentShader fragShader(oglplus::GLSLString(loadFileToString(fileLoc + "lines.frag")));

    vertShader.Compile();
    fragShader.Compile();

    //lineShader.AttachShader(vertShader);
    //lineShader.AttachShader(fragShader);
//
    //lineShader.Link();

    //uYPos = oglplus::Uniform<GLfloat>(lineShader, "yPos");
    //uXRot = oglplus::Uniform<GLfloat>(lineShader, "xRot");
    //uYRot = oglplus::Uniform<GLfloat>(lineShader, "yRot");
    //uPhase = oglplus::Uniform<GLfloat>(lineShader, "phase");
    //uVertical = oglplus::Uniform<GLboolean>(lineShader, "vert");

    //uYPos.SetValue(0);
    //uXRot.SetValue(0);
    //uYRot.SetValue(0);
    //uPhase.SetValue(0);
    //uVertical.SetValue(false);

    resize(winWidth, winHeight);

    //const int dim = 1;
    //const int N = 200;
    //lineData_v = Vector<GLfloat>(N * dim);
    //const float dt = 1. / N;
    //for (int i = 0; i < N; i++) {
    //    const float t = -1 + 2 * i * dt;
//
    //    const float x = t;
    //    lineData_v[i] = x; // X
    //}
//
    //line.Bind();
    //lineData.Bind(oglplus::Buffer::Target::Array);
    //oglplus::Buffer::Data(oglplus::Buffer::Target::Array, N*dim, &lineData_v[0]);
    //oglplus::VertexArrayAttrib(lineShader, "xPos").Setup<GLfloat>(N).Enable();
//
    //clock.restart();

    //spectrumTexture.loadFromFile("/home/joao/Developer/OpenGLStudies/Studies/FieldStudy/spectra/spectra.png");
    //spectrumSprite = sf::Sprite(spectrumTexture);
    //spectrumSprite.setPosition(0, 0);


}

void FieldStudy::doYourThing() {
    oglplus::Context gl;
    glViewport(0, 0, winWidth, winHeight);

    if(clock.getElapsedTimeSeconds() > 0.0016){
        phase += 0.025;
        uPhase.SetValue(phase);

        clock.restart();
    }

    //posSpaceVP.set();
    //lineShader.Bind();
    //const float dl = 0.056;
//
    //uVertical.SetValue(false);
    //for(float yPos=-1; yPos<=1; yPos += dl) {
    //    uYPos.SetValue(yPos);
    //    gl.DrawArrays(oglplus::PrimitiveType::LineStrip, 0, lineData_v.size());
    //}
//
    //uVertical.SetValue(true);
    //for(float yPos=-1; yPos<=1; yPos += dl) {
    //    uYPos.SetValue(yPos);
//
    //    line.draw();
    //}
//
    //fourierSpaceVP.set();

    //pendulumPhaseSpaceShader.bind();
    //quadVertices.draw();

    //glOrtho(-2, 2, -1, 1, -10, 10);
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

    {
        const float halfQuadSize = 0.8;

        Vector<GLfloat> quad(12);
        quad[0] = -halfQuadSize;
        quad[1] = -halfQuadSize;
        quad[2] = 0;
        quad[3] = -halfQuadSize;
        quad[4] = halfQuadSize;
        quad[5] = 0;
        quad[6] = halfQuadSize;
        quad[7] = halfQuadSize;
        quad[8] = 0;
        quad[9] = halfQuadSize;
        quad[10] = -halfQuadSize;
        quad[11] = 0;

        checkGLErrors("FieldStudy::resize 1");
        quadVertices.setData(quad, GL_QUADS, 3);
        checkGLErrors("FieldStudy::resize 2");
    }

    /*
    pendulumPhaseSpaceShader.setInt("width", (int)newWindowWidth);
    checkGLErrors("FieldStudy::resize 3");
    pendulumPhaseSpaceShader.setInt("height", (int)newWindowHeight);
    checkGLErrors("FieldStudy::resize 4");
     */
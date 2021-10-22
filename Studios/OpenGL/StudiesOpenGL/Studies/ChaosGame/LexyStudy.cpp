//
// Created by joao on 08/04/2021.
//

#include <cmath>
#include "LexyStudy.h"
#include "Studios/OpenGL/StudiesOpenGL/OpenGLUtils/Utils.h"
#include "Studios/OpenGL/StudiesOpenGL/util/Vector.h"

#include <nanogui/screen.h>
#include <nanogui/window.h>
#include <nanogui/label.h>
#include <nanogui/layout.h>
#include <nanogui/slider.h>

LexyStudy::LexyStudy(nanogui::Screen &screen) : screen(screen), chaosGame(
        0.7, 3), N(250000) {
    std::string fileLoc = "/home/joao/Developer/OpenGLStudies/shade/";
    pointsShader.loadShaders(fileLoc + "points.vert", fileLoc + "points.frag");

    auto size = screen.size();
    resize(size.x(), size.y());

    Clock timer; timer.restart();
    chaosGame.generatePointSet({0, 0, 0}, N, true, bias);
    float elTime = timer.getElapsedTimeSeconds();
    std::cout << "Point set generated in " << elTime << " sec for " << N << " points.";

    clock.restart();

    {
        window = new nanogui::Window(&screen, "");
        window->setPosition({15, 15});
        window->setFixedWidth(200);
        window->setLayout(new nanogui::GroupLayout(5, 5, 0, 0));
        nanogui::Label *l = new nanogui::Label(window, "Bias", "sans-bold");
        l->setFontSize(16);
        nanogui::Slider *slider = new nanogui::Slider(window);

        slider->setRange({0, 1});
        slider->setValue(0.5f);
        float &bias = this->bias;
        int &nDraw = this->nDraw;
        slider->setCallback([&bias, this, &nDraw](float value) {
            bias = value;
            this->chaosGame.generatePointSet({0, 0, 0,}, N, true, bias);
            nDraw = 100;
        });

        screen.performLayout();
    }
}

void LexyStudy::doYourThing() {
    auto size = screen.size();
    glViewport(0, 0, size.x(), size.y());
    glPointSize(1);

    if(clock.getElapsedTimeSeconds() > 0.001) {
        nDraw *= 1.03;
        clock.restart();
    }

    pointsShader.bind();
    chaosGame.draw(nDraw);
}

/*
void LexyStudy::event(sf::Event &event) {
    if(event.type == sf::Event::Resized)
        resize(event.size.width, event.size.height);
    else if(event.type == sf::Event::MouseMoved){
        if(sf::Mouse::isButtonPressed(sf::Mouse::Left)){

        }
    }
    else if(event.type == sf::Event::MouseButtonPressed) {
        sf::Vector2i mouse = sf::Mouse::getPosition();


    }
}

void LexyStudy::resize(GLuint newWindowWidth, GLuint newWindowHeight) {
    winWidth = newWindowWidth;
    winHeight = newWindowHeight;
}
*/

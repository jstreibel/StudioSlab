

#include "OutputOpenGL.h"


#include "Phys/Numerics/Allocator.h"


using namespace Base;

OutputOpenGL::OutputOpenGL(String channelName, int stepsBetweenDraws)
    : OutputChannel("OpenGL output", stepsBetweenDraws) { }

void OutputOpenGL::_out(const OutputPacket &outInfo){ /* Do nothing */ }

auto OutputOpenGL::notifyIntegrationHasFinished(const OutputPacket &theVeryLastOutputInformation) -> bool {
    return finishFrameAndRender();
}

bool OutputOpenGL::finishFrameAndRender() {
    for(auto *anim : animations) anim->step(frameTimer.getElTime_sec());

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();
    {
        {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glEnable(GL_POINT_SMOOTH);
            glEnable(GL_LINE_SMOOTH);
            glDisable(GL_DEPTH_TEST);
        }

        draw();
    }
    glPopMatrix();

    frameTimer.reset();
    return true;
}

void OutputOpenGL::notifyRender() {
    Window::notifyRender();

    finishFrameAndRender();
}

void OutputOpenGL::draw() {
    Window::draw();
}





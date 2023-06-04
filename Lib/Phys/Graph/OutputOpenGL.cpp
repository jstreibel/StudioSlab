

#include "OutputOpenGL.h"


#include "Phys/Numerics/Allocator.h"
#include "Phys/Numerics/Output/Plugs/Plug.h"
#include "Phys/Numerics/Output/Plugs/Plug.h"


using namespace Base;

Graphics::OutputOpenGL::OutputOpenGL(String channelName, int stepsBetweenDraws)
    : Numerics::OutputSystem::Plug("OpenGL output", stepsBetweenDraws) { }

void Graphics::OutputOpenGL::_out(const OutputPacket &outInfo){ /* Do nothing */ }

auto Graphics::OutputOpenGL::notifyIntegrationHasFinished(const OutputPacket &theVeryLastOutputInformation) -> bool {
    return finishFrameAndRender();
}

bool Graphics::OutputOpenGL::finishFrameAndRender() {
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

bool Graphics::OutputOpenGL::notifyRender() {
    Window::notifyRender();

    finishFrameAndRender();
}

void Graphics::OutputOpenGL::draw() {
    Window::draw();
}





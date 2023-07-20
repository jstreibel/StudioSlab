

#include <GL/gl.h>
#include "OutputOpenGL.h"

#include "Common/Log/Log.h"


using namespace Base;

Graphics::OutputOpenGL::OutputOpenGL(const NumericParams &params, Str channelName, int stepsBetweenDraws)
    : Numerics::OutputSystem::Socket(params, "OpenGL output", stepsBetweenDraws), Window() {
    EventListener::addResponder(&panel);

    Log::Info() << "Graphic monitor instantiated. Channel name: '" << channelName << "'." << Log::Flush;
}

void Graphics::OutputOpenGL::_out(const OutputPacket &outInfo, const NumericParams &params){
    t = outInfo.getSimTime();
    step = outInfo.getSteps();
}

auto Graphics::OutputOpenGL::notifyIntegrationHasFinished(const OutputPacket &theVeryLastOutputInformation, const NumericParams &params) -> bool {
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

bool Graphics::OutputOpenGL::notifyRender(float elTime_msec) {
    Window::notifyRender(elTime_msec);

    finishFrameAndRender();

    return true;
}

void Graphics::OutputOpenGL::draw() {
    Window::draw();
}

bool Graphics::OutputOpenGL::notifyKeyboard(unsigned char key, int x, int y) {
    if(key == '=') {
        setnSteps(getnSteps()+1);
        Log::Debug("nSteps=") << getnSteps();
        return true;
    } else if(key == '+') {
        setnSteps(getnSteps()*1.1);
        Log::Debug("nSteps=") << getnSteps();
        return true;
    } else if(key == '-') {
        setnSteps(getnSteps()-1);
        Log::Debug("nSteps=") << getnSteps();
        return true;
    } else if(key == '_') {
        setnSteps(getnSteps()/1.1);
        Log::Debug("nSteps=") << getnSteps();
        return true;
    }

    return EventListener::notifyKeyboard(key, x, y);
}

void Graphics::OutputOpenGL::notifyReshape(int newWinW, int newWinH) {
    Window::notifyReshape(newWinW, newWinH);

    panel.notifyReshape(newWinW, newWinH);
}

bool Graphics::OutputOpenGL::notifyScreenReshape(int newScreenWidth, int newScreenHeight) {
    panel.notifyScreenReshape(newScreenWidth, newScreenHeight);
    panel.notifyReshape(newScreenWidth, newScreenHeight);

    return Window::notifyScreenReshape(newScreenWidth, newScreenHeight);
}





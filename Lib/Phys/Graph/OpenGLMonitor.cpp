//
// Created by joao on 8/1/23.
//

#include <GL/gl.h>
#include "OpenGLMonitor.h"

#include "Base/Tools/Log.h"


using namespace Base;

#define CLEAR_BUFFERS false

Graphics::OpenGLMonitor::OpenGLMonitor(const NumericParams &params, Str channelName, int stepsBetweenDraws)
        : Numerics::OutputSystem::Socket(params, "OpenGL output", stepsBetweenDraws), Window() {
    EventListener::addResponder(&panel);

    panel.addWindow(&stats);

    Log::Info() << "Graphic monitor instantiated. Channel name: '" << channelName << "'." << Log::Flush;
}

void Graphics::OpenGLMonitor::_out(const OutputPacket &outInfo){
    t = outInfo.getSimTime();
    step = outInfo.getSteps();
}

auto Graphics::OpenGLMonitor::notifyIntegrationHasFinished(const OutputPacket &theVeryLastOutputInformationOStream) -> bool {
    return finishFrameAndRender();
}

void Graphics::OpenGLMonitor::writeStats() {
    static auto timer = Timer();
    auto elTime = timer.getElTime_msec();
    timer = Timer();

    const auto &p = params;
    const auto L = p.getL();
    const auto N = p.getN();
    const auto h = p.geth();

    static Count lastStep=0;
    auto dt = params.getdt();

    fix FPS = 1e3/elTime;

    stats.addVolatileStat(Str("t = ")    + ToStr(t, 4) + "/" + ToStr(params.gett(), 4));
    stats.addVolatileStat(Str("step = ") + ToStr(step) + "/" + ToStr(params.getn()));
    stats.addVolatileStat(Str("dt = ")   + ToStr(dt, 2, true));
    stats.addVolatileStat(Str("Steps/sample: ") + ToStr(lastData.getSteps() - lastStep));
    stats.addVolatileStat(Str("Steps/sec: ") + ToStr(getnSteps()/(elTime*1e-3), 0));
    stats.addVolatileStat(Str("FPS (samples/sec): ") + ToStr(FPS, 0));
    stats.addVolatileStat(Str("<\\br>"));
    stats.addVolatileStat(Str("L = ") + ToStr(L));
    stats.addVolatileStat(Str("N = ") + ToStr(N));
    stats.addVolatileStat(Str("h = ") + ToStr(h, 4, true));

    fix minFPS = 58, maxFPS = 59;
    if     ( FPS >= maxFPS ) setnSteps(getnSteps()+1);
    else if( FPS <= minFPS ) setnSteps(getnSteps()-1);

    lastStep = lastData.getSteps();
}

bool Graphics::OpenGLMonitor::finishFrameAndRender() {
    for(auto *anim : animations) anim->step(frameTimer.getElTime_sec());

    if(CLEAR_BUFFERS) glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();
    {
        {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glEnable(GL_POINT_SMOOTH);
            glEnable(GL_LINE_SMOOTH);
            glDisable(GL_DEPTH_TEST);
        }

        assert(lastData.hasValidData());

        writeStats();
        draw();
        panel.draw();
    }
    glPopMatrix();

    frameTimer.reset();
    return true;
}

bool Graphics::OpenGLMonitor::notifyRender(float elTime_msec) {
    Window::notifyRender(elTime_msec);

    finishFrameAndRender();

    return true;
}

bool Graphics::OpenGLMonitor::notifyKeyboard(unsigned char key, int x, int y) {
    if(key == '=') {
        setnSteps(getnSteps()+1);
        return true;
    } else if(key == '+') {
        setnSteps(getnSteps()*1.1);
        return true;
    } else if(key == '-') {
        setnSteps(getnSteps()-1);
        return true;
    } else if(key == '_') {
        setnSteps(getnSteps()/1.1);
        return true;
    }

    return EventListener::notifyKeyboard(key, x, y);
}

void Graphics::OpenGLMonitor::notifyReshape(int newWinW, int newWinH) {
    Window::notifyReshape(newWinW, newWinH);

    panel.notifyReshape(newWinW, newWinH);
}

bool Graphics::OpenGLMonitor::notifyScreenReshape(int newScreenWidth, int newScreenHeight) {
    panel.notifyScreenReshape(newScreenWidth, newScreenHeight);
    panel.notifyReshape(newScreenWidth, newScreenHeight);

    return Window::notifyScreenReshape(newScreenWidth, newScreenHeight);
}






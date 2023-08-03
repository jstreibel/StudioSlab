//
// Created by joao on 8/1/23.
//

#include <GL/gl.h>
#include "Monitor.h"

#include "Common/Log/Log.h"


using namespace Base;

Graphics::Monitor::Monitor(const NumericParams &params, Str channelName, int stepsBetweenDraws)
        : Numerics::OutputSystem::Socket(params, "OpenGL output", stepsBetweenDraws), Window() {
    EventListener::addResponder(&panel);

    panel.addWindow(&stats);

    Log::Info() << "Graphic monitor instantiated. Channel name: '" << channelName << "'." << Log::Flush;
}

void Graphics::Monitor::_out(const OutputPacket &outInfo){
    t = outInfo.getSimTime();
    step = outInfo.getSteps();
}

auto Graphics::Monitor::notifyIntegrationHasFinished(const OutputPacket &theVeryLastOutputInformationOStream) -> bool {
    return finishFrameAndRender();
}

void Graphics::Monitor::writeStats() {
    static auto timer = Timer();
    auto elTime = timer.getElTime_msec();
    timer = Timer();

    const auto &p = params;
    const auto L = p.getL();
    const auto N = p.getN();
    const auto h = p.geth();

    static auto lastStep=0;
    auto dt = params.getdt();

    stats.addVolatileStat(Str("t = ")    + ToStr(t, 4) + "/" + ToStr(params.gett(), 4));
    stats.addVolatileStat(Str("step = ") + ToStr(step) + "/" + ToStr(params.getn()));
    stats.addVolatileStat(Str("dt = ")   + ToStr(dt, 2, true));
    stats.addVolatileStat(Str("Steps/frame: ") + std::to_string(lastData.getSteps() - lastStep));
    stats.addVolatileStat(Str("Steps/sec: ") + ToStr(getnSteps()/(elTime*1e-3)));
    stats.addVolatileStat(Str("FPS: ") + ToStr(1/(elTime*1e-3)));
    stats.addVolatileStat(Str("<\\br>"));
    stats.addVolatileStat(Str("L = ") + ToStr(L));
    stats.addVolatileStat(Str("N = ") + ToStr(N));
    stats.addVolatileStat(Str("h = ") + ToStr(h, 4, true));

    lastStep = lastData.getSteps();
}

bool Graphics::Monitor::finishFrameAndRender() {
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

        assert(lastData.hasValidData());
        writeStats();
        draw();
        panel.draw();
    }
    glPopMatrix();

    frameTimer.reset();
    return true;
}

bool Graphics::Monitor::notifyRender(float elTime_msec) {
    Window::notifyRender(elTime_msec);

    finishFrameAndRender();

    return true;
}

bool Graphics::Monitor::notifyKeyboard(unsigned char key, int x, int y) {
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

void Graphics::Monitor::notifyReshape(int newWinW, int newWinH) {
    Window::notifyReshape(newWinW, newWinH);

    panel.notifyReshape(newWinW, newWinH);
}

bool Graphics::Monitor::notifyScreenReshape(int newScreenWidth, int newScreenHeight) {
    panel.notifyScreenReshape(newScreenWidth, newScreenHeight);
    panel.notifyReshape(newScreenWidth, newScreenHeight);

    return Window::notifyScreenReshape(newScreenWidth, newScreenHeight);
}






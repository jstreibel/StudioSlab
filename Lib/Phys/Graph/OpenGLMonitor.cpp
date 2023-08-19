//
// Created by joao on 8/1/23.
//

#include <GL/gl.h>
#include "OpenGLMonitor.h"

#include "Base/Tools/Log.h"


using namespace Base;

#define AUTO_ADJUST_SAMPLES_PER_SECOND false


Graphics::OpenGLMonitor::OpenGLMonitor(const NumericConfig &params, const Str& channelName, int stepsBetweenDraws)
        : Numerics::OutputSystem::Socket(params, channelName, stepsBetweenDraws), Window() {
    EventListener::addResponder(&panel);

    panel.addWindow(&stats);

    Log::Info() << "Graphic monitor instantiated. Channel name: '" << channelName << "'." << Log::Flush;
}

void Graphics::OpenGLMonitor::handleOutput(const OutputPacket &outInfo){
    t = outInfo.getSimTime();
    step = outInfo.getSteps();
}

auto Graphics::OpenGLMonitor::notifyIntegrationHasFinished(const OutputPacket &theVeryLastOutputInformationOStream) -> bool {
    // return finishFrameAndRender();
    return true;
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

    auto avgFPS = 0.0;
    auto avgSPS = 0.0;
    fix SPs = lastData.getSteps() - lastStep;
    {
        fix FPS = 1e3/elTime;
        fix SPS = getnSteps()/(elTime*1e-3);

        static std::vector<Real> FPSmeasures;
        static std::vector<Real> SPSmeasures; // steps per second

        fix MAX_AVG_SAMPLES = 200UL;

        FPSmeasures.emplace_back(FPS);
        SPSmeasures.emplace_back(SPS);

        fix count = FPSmeasures.size();
        fix countMin = count > MAX_AVG_SAMPLES ? count-MAX_AVG_SAMPLES : 0;
        fix total = count-countMin;

        for(auto index=countMin; index<count; index++) {
            avgFPS += FPSmeasures[index];
            avgSPS += SPSmeasures[index];
        }
        avgFPS /= (Real)total;
        avgSPS /= (Real)total;

        if(AUTO_ADJUST_SAMPLES_PER_SECOND) {
            fix minFPS = 58, maxFPS = 59;
            if      (avgFPS >= maxFPS) setnSteps(getnSteps() + 1);
            else if (avgFPS <= minFPS) setnSteps(getnSteps() - 1);
        }
    }

    stats.addVolatileStat(Str("t = ")    + ToStr(t, 4) + "/" + ToStr(params.gett(), 4));
    stats.addVolatileStat(Str("step = ") + ToStr(step) + "/" + ToStr(params.getn()));
    stats.addVolatileStat(Str("dt = ")   + ToStr(dt, 2, true));
    stats.addVolatileStat(Str("Steps/sample: ") + ToStr(SPs));
    stats.addVolatileStat(Str("Steps/sec: ") + ToStr(avgSPS, 0));
    stats.addVolatileStat(Str("FPS (samples/sec): ") + ToStr(avgFPS, 1));
    stats.addVolatileStat(Str("<\\br>"));
    stats.addVolatileStat(Str("L = ") + ToStr(L));
    stats.addVolatileStat(Str("N = ") + ToStr(N));
    stats.addVolatileStat(Str("h = ") + ToStr(h, 4, true));

    lastStep = lastData.getSteps();
}

bool Graphics::OpenGLMonitor::finishFrameAndRender() {
    for(auto *anim : animations) anim->step(frameTimer.getElTime_sec());

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






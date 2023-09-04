//
// Created by joao on 8/1/23.
//

#include "OpenGLMonitor.h"

#include "Core/Tools/Log.h"
#include "Core/Backend/GUIBackend.h"


using namespace Core;

#define MAX_AVG_SAMPLES (2*60UL)
#define MIN_FPS 24
#define MAX_FPS 30

Core::Graphics::OpenGLMonitor::OpenGLMonitor(const NumericConfig &params, const Str& channelName, int stepsBetweenDraws)
        : Numerics::OutputSystem::Socket(params, channelName, stepsBetweenDraws), Window() {
    EventListener::addResponder(&panel);

    panel.addWindow(&stats);

    Log::Info() << "Graphic monitor instantiated. Channel name: '" << channelName << "'." << Log::Flush;
}

void Core::Graphics::OpenGLMonitor::handleOutput(const OutputPacket &outInfo){
    t = outInfo.getSimTime();
    step = outInfo.getSteps();

    GUIBackend::GetInstance().requestRender();
}

void Core::Graphics::OpenGLMonitor::writeStats() {
    static bool hasFinished = false;
    hasFinished = !(lastData.getSteps()<params.getn());


    static auto timer = Timer();
    auto elTime = timer.getElTime_msec();
    timer = Timer();

    const auto &p = params;
    const auto L = p.getL();
    const auto N = p.getN();
    const auto h = p.geth();

    static Count lastStep=0;
    auto dt = params.getdt();

    fix currStep = step;

    fix FPS = 1e3/elTime;           // Frames/samples per second
    fix SPs = currStep - lastStep;  // Steps per frame/sample
    fix SPS = (Real)SPs*FPS;        // Steps per second

    auto avgFPS = .0;
    auto avgSPS = .0; // careful with division by zero below
    auto avgSPs = .0;
    if(step>0)
    {
        static std::vector<Real> FPSmeasures;
        static std::vector<Real> SPsmeasures; // steps per sample
        static std::vector<Real> SPSmeasures; // steps per second

        FPSmeasures.emplace_back(FPS);
        SPsmeasures.emplace_back(SPs);
        SPSmeasures.emplace_back(SPS);

        fix count = FPSmeasures.size();
        fix countMin = count > MAX_AVG_SAMPLES ? count-MAX_AVG_SAMPLES : 0;
        fix total = count-countMin;

        for(auto index=countMin; index<count; index++) {
            avgFPS += FPSmeasures[index];
            avgSPs += SPsmeasures[index];
            avgSPS += SPSmeasures[index];
        }
        avgFPS /= (Real)total;
        avgSPs /= (Real)total;
        avgSPS /= (Real)total;

        if(autoAdjustStepsPerSecond && !hasFinished) {
            static Count counter = 0;
            static fix baseNSteps = getnSteps();
            static let multiplier = 1;
            static fix modVal = Common::max(baseNSteps/5, 1);

            if(!(++counter%modVal))
            {
                if      (FPS >= MAX_FPS) {
                    setnSteps(baseNSteps * ++multiplier);
                    // setnSteps(getnSteps()+1);
                    autoAdjustStepsPerSecond = true;
                }
                else if (FPS <= MIN_FPS){
                    --multiplier;
                    if(multiplier<=0) multiplier = 1;
                    setnSteps(baseNSteps*multiplier);
                    // setnSteps(getnSteps()-1);
                    autoAdjustStepsPerSecond = true;
                }
            }
        }
    }

    fix stepsToFinish = params.getn() - step;
    fix timeToFinish = (int)(avgSPS==0.0 ? 0.0 : stepsToFinish/avgSPS);
    fix remainingTimeMin = timeToFinish / 60;
    fix remainingTimeSec = timeToFinish % 60;

    static auto totalTime = Timer();

    if(hasFinished) totalTime.stop();

    fix totalTimeIn_msec = (int)totalTime.getElTime_msec();
    fix totalTimeMSecs = (totalTimeIn_msec%1000);
    fix totalTimeSecs = (totalTimeIn_msec/1000)%60;
    fix totalTimeMins = (totalTimeIn_msec/1000)/60;

    stats.addVolatileStat(Str("t = ")    + ToStr(t, 4) + "/" + ToStr(params.gett(), 4));
    stats.addVolatileStat(Str("step = ") + ToStr(step) + "/" + ToStr(params.getn()));
    stats.addVolatileStat(Str("dt = ")   + ToStr(dt, 2, true));
    stats.addVolatileStat("");
    stats.addVolatileStat(Str("Steps/sample: ") + ToStr(avgSPs) + " (" + ToStr(getnSteps()) + ")");
    stats.addVolatileStat(Str("Steps/sec: ") + ToStr(avgSPS, 0));
    stats.addVolatileStat(Str("FPS (samples/sec): ") + ToStr(avgFPS, 1));
    if(currStep!=lastStep || hasFinished)
        stats.addVolatileStat(Str("Finish in ")
            + (remainingTimeMin < 10 ? "0" : "")
            + ToStr(remainingTimeMin) + "m"
            + (remainingTimeSec < 10 ? "0" : "")
            + ToStr(remainingTimeSec) + "s");
    else
        stats.addVolatileStat(Str("Finish in ∞s"));
    fix elTimeMins_str = (totalTimeMins < 10 ? "0" : "") + ToStr(totalTimeMins) + "m";
    fix elTimeSecs_str = (totalTimeSecs < 10 ? "0" : "") + ToStr(totalTimeSecs) + "s";
    fix elTimeMSecs_str = Str(totalTimeMSecs < 100 ? "0" : "") + (totalTimeMSecs < 10 ? "0" : "") + ToStr(totalTimeMSecs) + "ms";
    stats.addVolatileStat(Str("El. time ") + elTimeMins_str + elTimeSecs_str + elTimeMSecs_str);
    stats.addVolatileStat(Str("<\\br>"));
    stats.addVolatileStat(Str("L = ") + ToStr(L));
    stats.addVolatileStat(Str("N = ") + ToStr(N));
    stats.addVolatileStat(Str("h = ") + ToStr(h, 4, true));

    lastStep = step;
}

bool Core::Graphics::OpenGLMonitor::notifyRender(float elTime_msec) {
    assert(lastData.hasValidData());

    for(auto *anim : animations) anim->step(frameTimer.getElTime_sec());

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_LINE_SMOOTH);
    glDisable(GL_DEPTH_TEST);
    glPushMatrix();
    {
        writeStats();
        draw();
        EventListener::notifyRender(elTime_msec);   // here panel gets called.
        frameTimer.reset();
    }
    glPopMatrix();

    return true;
}

bool Core::Graphics::OpenGLMonitor::notifyKeyboard(unsigned char key, int x, int y) {
    static fix baseNSteps = getnSteps();
    static let multiplier = 1;

    if(false) {
        if (key == '=') {
            multiplier++;
            setnSteps(baseNSteps * multiplier);
        } else if (key == '-') {
            if (multiplier > 1) multiplier--;
            setnSteps(baseNSteps * multiplier);
        }
    } else {
        if (key == '=') {
            setnSteps(getnSteps() + 1);
            return true;
        } else if (key == '+') {
            auto nSteps = getnSteps();
            if (nSteps >= 10) setnSteps((int) (nSteps * 1.1));
            else setnSteps(10);
            return true;
        } else if (key == '-') {
            setnSteps(getnSteps() - 1);
            return true;
        } else if (key == '_') {
            setnSteps(getnSteps() / 1.1);
            return true;
        }
    }

    return EventListener::notifyKeyboard(key, x, y);
}

void Core::Graphics::OpenGLMonitor::notifyReshape(int newWinW, int newWinH) {
    Window::notifyReshape(newWinW, newWinH);

    panel.notifyReshape(newWinW, newWinH);
}

void Core::Graphics::OpenGLMonitor::setnSteps(int nSteps) {
    if(nSteps>0) autoAdjustStepsPerSecond = false;
    Socket::setnSteps(nSteps);
}

void Graphics::OpenGLMonitor::setAutoAdjust_nSteps(bool value) {
    autoAdjustStepsPerSecond = value;
}

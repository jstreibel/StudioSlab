//
// Created by joao on 8/1/23.
//

#include "BaseMonitor.h"

#include "Core/Tools/Log.h"
#include "Graphics/Backend/GraphicBackend.h"
#include "Core/Backend/BackendManager.h"


#define MAX_AVG_SAMPLES (2*60UL)
#define MIN_FPS 24
#define MAX_FPS 30


namespace Slab::Graphics {

    using Core::Log;

    BaseMonitor::BaseMonitor(const CountType MaxSteps, const Str &ChannelName, int StepsBetweenDraws)
    : Socket(ChannelName, StepsBetweenDraws), WindowPanel(FSlabWindowConfig{{}}), MaxSteps(MaxSteps)
    , GuiWindow(New<FGUIWindow>(FSlabWindowConfig(ChannelName)))
    {
        AddWindow(GuiWindow);
        SetColumnRelativeWidth(0, 0.1);

        Log::Status() << "Graphic monitor '" << ChannelName << "'. instantiated " << Log::Flush;
    }

    void BaseMonitor::handleOutput(const OutputPacket &outInfo) {
        step = outInfo.GetSteps();
    }

    void BaseMonitor::writeStats() {
        static bool hasFinished = false;
        static bool isPaused = false;
        static CountType lastStep = 0;
        hasFinished = !(LastPacket.GetSteps() < MaxSteps);

        fix currStep = step;

        isPaused = currStep == lastStep;

        static auto timer = Timer();
        auto elTime = timer.getElTime_msec();
        timer = Timer();

        // const auto &p = params;
        // const auto L = p.getL();
        // const auto N = p.getN();
        // const auto h = p.geth();


        fix FPS = 1e3 / elTime;           // Frames/samples per second
        fix SPs = currStep - lastStep;  // Steps per frame/sample
        fix SPS = (DevFloat) SPs * FPS;        // Steps per second

        auto avgFPS = .0;
        auto avgSPS = .0; // careful with division by zero below
        auto avgSPs = .0;
        if (step > 0) {
            static Vector<DevFloat> FPSmeasures;
            static Vector<DevFloat> SPsmeasures; // steps per sample
            static Vector<DevFloat> SPSmeasures; // steps per second

            FPSmeasures.emplace_back(FPS);
            SPsmeasures.emplace_back(SPs);
            SPSmeasures.emplace_back(SPS);

            fix count = FPSmeasures.size();
            fix countMin = count > MAX_AVG_SAMPLES ? count - MAX_AVG_SAMPLES : 0;
            fix total = count - countMin;

            for (auto index = countMin; index < count; index++) {
                avgFPS += FPSmeasures[index];
                avgSPs += SPsmeasures[index];
                avgSPS += SPSmeasures[index];
            }
            avgFPS /= (DevFloat) total;
            avgSPs /= (DevFloat) total;
            avgSPS /= (DevFloat) total;
        }

        fix stepsToFinish = MaxSteps - step;
        fix timeToFinish = (int) (avgSPS == 0.0 ? 0.0 : (DevFloat)stepsToFinish / avgSPS);
        fix remainingTimeMin = timeToFinish / 60;
        fix remainingTimeSec = timeToFinish % 60;

        static auto totalTime = Timer();

        if (hasFinished) totalTime.stop();

        fix totalTimeIn_msec = (int) totalTime.getElTime_msec();
        fix totalTimeMSecs = (totalTimeIn_msec % 1000);
        fix totalTimeSecs = (totalTimeIn_msec / 1000) % 60;
        fix totalTimeMins = (totalTimeIn_msec / 1000) / 60;
        
        GuiWindow->AddVolatileStat(Str("step = ") + ToStr(step) + "/" + ToStr(MaxSteps));
        // guiWindow.addVolatileStat(Str("dt = ") + ToStr(dt, 2, true));
        GuiWindow->AddVolatileStat("");
        GuiWindow->AddVolatileStat(Str("Steps/sample: ") + ToStr(avgSPs) + " (" + ToStr(getnSteps()) + ")");
        GuiWindow->AddVolatileStat(Str("Steps/sec: ") + ToStr(avgSPS, 0));
        GuiWindow->AddVolatileStat(Str("FPS (samples/sec): ") + ToStr(avgFPS, 1));
        if (currStep != lastStep || hasFinished)
            GuiWindow->AddVolatileStat(Str("Finish in ")
                                      + (remainingTimeMin < 10 ? "0" : "")
                                      + ToStr(remainingTimeMin) + "m"
                                      + (remainingTimeSec < 10 ? "0" : "")
                                      + ToStr(remainingTimeSec) + "s");
        else
            GuiWindow->AddVolatileStat(Str("Finish in ∞s"));
        fix elTimeMins_str = (totalTimeMins < 10 ? "0" : "") + ToStr(totalTimeMins) + "m";
        fix elTimeSecs_str = (totalTimeSecs < 10 ? "0" : "") + ToStr(totalTimeSecs) + "s";
        fix elTimeMSecs_str =
                Str(totalTimeMSecs < 100 ? "0" : "") + (totalTimeMSecs < 10 ? "0" : "") + ToStr(totalTimeMSecs) + "ms";
        GuiWindow->AddVolatileStat(Str("El. time ") + elTimeMins_str + elTimeSecs_str + elTimeMSecs_str);
        GuiWindow->AddVolatileStat(Str("<\\br>"));
        // guiWindow.addVolatileStat(Str("L = ") + ToStr(L));
        // guiWindow.addVolatileStat(Str("N = ") + ToStr(N));
        // guiWindow.addVolatileStat(Str("h = ") + ToStr(h, 4, true));

        lastStep = step;
    }

    void BaseMonitor::ImmediateDraw(const FPlatformWindow& PlatformWindow) {
        assert(LastPacket.hasValidData());

        {
            writeStats();
            WindowPanel::ImmediateDraw(PlatformWindow); // draw();
            frameTimer.reset();
        }
    }

    bool BaseMonitor::NotifyKeyboard(EKeyMap key, EKeyState state, EModKeys modKeys) {
        static fix baseNSteps = getnSteps();
        static let multiplier = 1;

        if (false) {
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
                setnSteps((int) (getnSteps() / 1.1));
                return true;
            }
        }

        return WindowPanel::NotifyKeyboard(key, state, modKeys);
    }

    FGUIWindow &BaseMonitor::getGUIWindow() const {
        return *GuiWindow;
    }


}
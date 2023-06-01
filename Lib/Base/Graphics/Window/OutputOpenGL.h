#ifndef OUTPUTOPENGL_H
#define OUTPUTOPENGL_H

#include "Phys/Numerics/Output/Channel/OutputChannel.h"

#include "Window.h"
#include "StatsDisplay.h"

#include <iostream>
#include <vector>

namespace Base {

    #define isOutputOpenGL(output) (dynamic_cast<Base::OutputOpenGL*>(output) != nullptr)

    class OutputOpenGL : public OutputChannel, public Window {
    protected:
        void draw() override;

    public:
        OutputOpenGL(String channelName="OpenGL output", int stepsBetweenDraws=1);



    // ********************* From Window ************** //
        void notifyRender() final override;
    // ********************* End Window ************** //


    // ********************* From OutputChannel ********************* //
        auto notifyIntegrationHasFinished(const OutputPacket &theVeryLastOutputInformation) -> bool override;
    protected:
        void _out(const OutputPacket &outInfo) override;
    // ********************* END OutputBase ********************** //


    public:
        void addAnimation(Animation *animation) {animations.push_back(animation); }
        void addStat(const String& stat, const Styles::Color color = {1, 1, 1}) {
            stats.addVolatileStat(stat, color);
        }

    private:
        bool finishFrameAndRender();
        std::vector<Animation*> animations;

    protected:
        Timer frameTimer = Timer();
        StatsDisplay stats;
    };
}

#endif // OUTPUTOPENGL_H

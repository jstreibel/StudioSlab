#ifndef OUTPUTOPENGL_H
#define OUTPUTOPENGL_H

#include "Base/Graphics/Window/WindowContainer/WindowPanel.h"
#include "Base/Graphics/Window/StatsDisplay.h"
#include "Phys/Numerics/Output/Plugs/Socket.h"

#include <iostream>
#include <vector>

namespace Graphics {

    class Monitor : public Numerics::OutputSystem::Socket, public Window {
        std::vector<Animation*> animations;

        void writeStats();
        bool finishFrameAndRender();

    protected:
        WindowPanel panel;
        Timer frameTimer = Timer();
        StatsDisplay stats;

        Real t;
        size_t step;


    public:
        typedef std::shared_ptr<Monitor> Ptr;

        Monitor(const NumericParams &params, Str channelName="OpenGL monitor", int stepsBetweenDraws=1);

        // ********************* From EventListener ************** //
        bool notifyRender(float elTime_msec) final override;
        bool notifyKeyboard(unsigned char key, int x, int y) override;
        void notifyReshape(int newWinW, int newWinH) override;
        bool notifyScreenReshape(int newScreenWidth, int newScreenHeight) override;
        // ********************* End EventListener *************** //


        // ********************* From Socket ********************* //
        auto notifyIntegrationHasFinished(const OutputPacket &theVeryLastOutputInformation) -> bool override;
    protected:
        void _out(const OutputPacket &outInfo) override;
        // ********************* END Socket ********************** //


    public:
        void addAnimation(Animation *animation) {animations.push_back(animation); }
        void addStat(const Str& stat, const Styles::Color color = {1, 1, 1}) {
            stats.addVolatileStat(stat, color);
        }

    };
}

#endif // OUTPUTOPENGL_H

#ifndef OUTPUTOPENGL_H
#define OUTPUTOPENGL_H

#include "Base/Graphics/Window/Window.h"
#include "Base/Graphics/Window/StatsDisplay.h"
#include "Phys/Numerics/Output/Plugs/Socket.h"

#include <iostream>
#include <vector>

namespace Graphics {

    class OutputOpenGL : public Numerics::OutputSystem::Socket, public Window {
        bool finishFrameAndRender();
        std::vector<Animation*> animations;

    protected:
        Timer frameTimer = Timer();
        StatsDisplay stats;
        void draw() override;

        Real t;
        size_t step;

    public:
        typedef std::shared_ptr<OutputOpenGL> Ptr;

        OutputOpenGL(Str channelName="OpenGL output", int stepsBetweenDraws=1);

        // ********************* From EventListener ************** //
        bool notifyRender() final override;
        bool notifyKeyboard(unsigned char key, int x, int y) override;
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

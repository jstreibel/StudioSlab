#ifndef OUTPUTOPENGL_H
#define OUTPUTOPENGL_H

#include <Phys/Numerics/Output/Channel/OutputChannel.h>

#include <Base/Backend/GLUT/GLUTEventListener.h>
#include <Base/Graphics/WindowManagement/WindowContainer/WindowPanel.h>
#include <Base/Graphics/Artists/StatsDisplay.h>

// STD includes
#include <iostream>
#include <vector>

namespace Base {

    #define isOutputOpenGL(output) (dynamic_cast<Base::OutputOpenGL*>(output) != nullptr)

    class OutputOpenGL : public OutputChannel, public GLUTEventListener {
    public:
        OutputOpenGL(String channelName="OpenGL output", int stepsBetweenDraws=1);
        ~OutputOpenGL() override;

        auto needDraw() const -> bool;
        virtual void draw();
        virtual IntPair getWindowSizeHint();


    // ********************* From GLUTEventListener ************** //
        void notifyRender() final override;

        void notifyReshape(int width, int height) override;
    // ********************* End GLUTEventListener ************** //
    // ********************* From OutputChannel ********************* //
        auto notifyIntegrationHasFinished(const OutputPacket &theVeryLastOutputInformation) -> bool override;
    protected:
        void _out(const OutputPacket &outInfo) override;
    // ********************* END OutputBase ********************** //


    public:
        void addAnimation(Animation *animation) {animations.push_back(animation); }
        void addStat(const String& stat, const Color color = {1, 1, 1}) {
            stats.addVolatileStat(stat, color);
        }

    private:
        bool finishFrameAndRender();
        std::vector<Animation*> animations;


    protected:
        int osWindowWidth, osWindowHeight;

        Timer frameTimer = Timer();

        WindowPanel *panel;
        StatsDisplay stats;
    };
}

#endif // OUTPUTOPENGL_H

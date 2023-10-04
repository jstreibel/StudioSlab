#ifndef OUTPUTOPENGL_H
#define OUTPUTOPENGL_H

#include "Graphics/Window/WindowContainer/WindowPanel.h"
#include "Graphics/Window/GUIWindow.h"
#include "Math/Numerics/Output/Plugs/Socket.h"

#include <iostream>
#include <vector>

namespace Graphics {

    class OpenGLMonitor : public Numerics::OutputSystem::Socket, public Window {
        void writeStats();

        bool autoAdjustStepsPerSecond = true;

    protected:
        WindowPanel panel;
        Timer frameTimer = Timer();
        GUIWindow stats;

        Real t=.0;
        size_t step=0;


    public:
        typedef std::shared_ptr<OpenGLMonitor> Ptr;

        explicit OpenGLMonitor(const NumericConfig &params, const Str& channelName="OpenGL monitor", int stepsBetweenDraws=1);

        void setAutoAdjust_nSteps(bool value);

        // ********************* From EventListener ************** //
        bool notifyRender() final ;
        bool notifyKeyboard(Core::KeyMap key, Core::KeyState state, Core::ModKeys modKeys) override;
        void notifyReshape(int newWinW, int newWinH) override;
        // ********************* End EventListener *************** //


        // ********************* From Socket ********************* //
    protected:
        void handleOutput(const OutputPacket &outInfo) override;

    public:
        auto setnSteps(int nSteps) -> void override;
        // ********************* END Socket ********************** //

    };
}

#endif // OUTPUTOPENGL_H

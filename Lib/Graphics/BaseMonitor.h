#ifndef OUTPUTOPENGL_H
#define OUTPUTOPENGL_H

#include "Graphics/Window/WindowContainer/WindowPanel.h"
#include "Graphics/Window/GUIWindow.h"
#include "Math/Numerics/Socket.h"
#include "Graphics/Window/WindowContainer/WindowRow.h"
#include "Graphics/Window/WindowContainer/WindowColumn.h"

#include <iostream>
#include <vector>

namespace Slab::Graphics {

    using namespace Math;

    class BaseMonitor : public Socket, public WindowPanel {
        void writeStats();

    protected:
        Timer frameTimer = Timer();
        GUIWindow guiWindow;

        const Count max_steps;

        size_t step=0;

    public:
        typedef std::shared_ptr<BaseMonitor> Ptr;

        explicit BaseMonitor(Count max_steps,
                             const Str& channelName="OpenGL monitor",
                             int stepsBetweenDraws=10);

        GUIWindow &getGUIWindow();

        void draw() override;
        bool notifyKeyboard(KeyMap key, KeyState state, ModKeys modKeys) override;

        // ********************* From Socket ********************* //
        void handleOutput(const OutputPacket &outInfo) override;
        // ********************* END Socket ********************** //

    };

    DefinePointers(BaseMonitor)
}

#endif // OUTPUTOPENGL_H

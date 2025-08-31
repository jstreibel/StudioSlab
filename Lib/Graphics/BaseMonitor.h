#ifndef OUTPUTOPENGL_H
#define OUTPUTOPENGL_H

#include "Graphics/Window/WindowContainer/WindowPanel.h"
#include "Graphics/Window/GUIWindow.h"
#include "Math/Numerics/OutputChannel.h"
#include "Graphics/Window/WindowContainer/WindowRow.h"

#include <vector>

namespace Slab::Graphics {

    using namespace Math;

    class BaseMonitor : public FOutputChannel, public WindowPanel {
        void writeStats();

    protected:
        FTimer frameTimer = FTimer();
        TPointer<FGUIWindow> GuiWindow;

        const CountType MaxSteps;

        size_t step=0;

    public:
        typedef std::shared_ptr<BaseMonitor> Ptr;

        explicit BaseMonitor(CountType MaxSteps,
                             const Str& ChannelName="OpenGL monitor",
                             int StepsBetweenDraws=10);

        [[nodiscard]] FGUIWindow &getGUIWindow() const;

        void ImmediateDraw(const FPlatformWindow&) override;
        bool NotifyKeyboard(EKeyMap key, EKeyState state, EModKeys modKeys) override;

        // ********************* From Socket ********************* //
        void HandleOutput(const FOutputPacket &outInfo) override;
        // ********************* END Socket ********************** //

    };

    DefinePointers(BaseMonitor)
}

#endif // OUTPUTOPENGL_H

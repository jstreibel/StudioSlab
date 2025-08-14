//
// Created by joao on 11/3/24.
//

#ifndef STUDIOSLAB_IMGUICONTEXT_H
#define STUDIOSLAB_IMGUICONTEXT_H

#include "Graphics/Modules/GUIModule/GUIContext.h"
#include "Graphics/Backend/PlatformWindow.h"
#include "3rdParty/ImGui.h"

#include "Utils/Arrays.h"

#include <functional>

namespace Slab::Graphics {

    struct FCallSet {
        using RawSystemWindowPointer = void*;

        using InitContextCall = std::function<void(const FCallSet& Call)>;
        using KillContextCall = std::function<void(void)>;
        using NewFrameCall    = std::function<void(void)>;

        RawSystemWindowPointer r_SystemWindow;

        InitContextCall Init;
        KillContextCall End;
        FDrawCall       Draw;
        NewFrameCall    NewFrame;
    };

    class FImGuiContext final : public FGUIContext {
        ImGuiContext *r_Context = nullptr;

        FCallSet ImplementationCalls;

    public:
        explicit FImGuiContext(FCallSet);
        ~FImGuiContext() override = default;

        [[nodiscard]] DevFloat GetFontSize() const;

        void AddMainMenuItem(MainMenuItem) override;

        void *GetContextPointer() override;

        bool NotifyKeyboard(EKeyMap key, EKeyState state, EModKeys modKeys) override;

        bool NotifyCharacter(UInt codepoint) override;

        void CursorEntered(bool b) override;

        bool NotifyMouseButton(EMouseButton button, EKeyState state, EModKeys keys) override;

        bool NotifyMouseMotion(int x, int y, int dx, int dy) override;

        bool NotifyMouseWheel(double dx, double dy) override;

        bool NotifySystemWindowReshape(int w, int h) override;

        bool NotifyRender(const FPlatformWindow&) override;

        void Bind() override;
        void NewFrame() override;
        void Render() override;
    };

} // Slab::Graphics

#endif //STUDIOSLAB_IMGUICONTEXT_H

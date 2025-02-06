//
// Created by joao on 11/3/24.
//

#ifndef STUDIOSLAB_IMGUICONTEXT_H
#define STUDIOSLAB_IMGUICONTEXT_H

#include "Graphics/Modules/GUIModule/GUIContext.h"
#include "Graphics/Backend/SystemWindow.h"
#include "3rdParty/ImGui.h"

#include "Utils/Arrays.h"

#include <functional>

namespace Slab::Graphics {

    struct CallSet {
        using RawSystemWindowPointer = void*;

        using InitContextCall = std::function<void(RawSystemWindowPointer)>;
        using KillContextCall = std::function<void(void)>;
        using NewFrameCall    = std::function<void(void)>;

        InitContextCall Init;
        KillContextCall End;
        DrawCall        Draw;
        NewFrameCall    NewFrame;
    };

    class SlabImGuiContext final : public GUIContext {
        ImGuiContext *context = nullptr;

        CallSet call_set;

    public:
        explicit SlabImGuiContext(ParentSystemWindow, CallSet);
        ~SlabImGuiContext() override = default;

        Real getFontSize() const;

        void AddMainMenuItem(MainMenuItem) override;

        void Bind() override;
        void NewFrame() override;
        void Render() const override;

        void *GetContextPointer() override;

        bool notifyKeyboard(KeyMap key, KeyState state, ModKeys modKeys) override;

        bool notifyCharacter(UInt codepoint) override;

        void cursorEntered(bool b) override;

        bool notifyMouseButton(MouseButton button, KeyState state, ModKeys keys) override;

        bool notifyMouseMotion(int x, int y, int dx, int dy) override;

        bool notifyMouseWheel(double dx, double dy) override;

        bool notifySystemWindowReshape(int w, int h) override;

        bool notifyRender() override;
    };

} // Slab::Graphics

#endif //STUDIOSLAB_IMGUICONTEXT_H

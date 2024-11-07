//
// Created by joao on 11/3/24.
//

#ifndef STUDIOSLAB_IMGUICONTEXT_H
#define STUDIOSLAB_IMGUICONTEXT_H

#include "Graphics/Modules/GUIModule/GUIContext.h"
#include "3rdParty/ImGui.h"

#include "Utils/Arrays.h"

#include <functional>

namespace Slab::Graphics {

    using DrawCall = std::function<void(void)>;

    class SlabImGuiContext : public GUIContext {
        ImGuiContext *context = nullptr;

        Vector<DrawCall> external_draws;


    public:
        explicit SlabImGuiContext(ImGuiContext *context);
        ~SlabImGuiContext() override = default;

        Real getFontSize() const;

        void Bind();
        void NewFrame() override;

        void AddDrawCall(const DrawCall&);

        bool notifyKeyboard(KeyMap key, KeyState state, ModKeys modKeys) override;

        bool notifyMouseButton(MouseButton button, KeyState state, ModKeys keys) override;

        bool notifyMouseMotion(int x, int y, int dx, int dy) override;

        bool notifyMouseWheel(double dx, double dy) override;

        bool notifyFilesDropped(StrVector paths) override;

        bool notifySystemWindowReshape(int w, int h) override;

        bool notifyRender() override;
    };

} // Slab::Graphics

#endif //STUDIOSLAB_IMGUICONTEXT_H

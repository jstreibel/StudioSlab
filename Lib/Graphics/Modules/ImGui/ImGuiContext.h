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

    using ExternalDraw = std::function<void(void)>;

    class SlabImGuiContext : public GUIContext {
        ImGuiContext *context = nullptr;

        Vector<ExternalDraw> external_draws;


    public:
        explicit SlabImGuiContext(ImGuiContext *context);
        ~SlabImGuiContext() = default;

        ImGuiContext *
        getNativeContext();

        Real getFontSize() const;

        void AddExternalDraw(const ExternalDraw&);

        void Bind();
        void NewFrame() override;
        void Render() override;
    };

} // Slab::Graphics

#endif //STUDIOSLAB_IMGUICONTEXT_H

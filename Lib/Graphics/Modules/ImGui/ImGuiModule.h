//
// Created by joao on 22/09/23.
//

#ifndef STUDIOSLAB_IMGUIMODULE_H
#define STUDIOSLAB_IMGUIMODULE_H

#include "Core/Backend/Implementations.h"
#include "Core/Backend/Modules/Module.h"

#include "Graphics/Modules/GraphicsModule.h"

#include "3rdParty/ImGui.h"

namespace Slab::Graphics {

    class ImGuiModule : public GraphicsModule {
        void generalInitialization();
        void buildFonts();

        ImGuiContext *m_Context = nullptr;

    protected:
        bool showDemos = false;

        void finishInitialization();
        explicit ImGuiModule();
        ~ImGuiModule() override;

    public:
        static ImGuiModule* BuildModule();

        void beginRender() override;

        void endRender() override;

        ImGuiContext* getContext();
    };

} // Core

#endif //STUDIOSLAB_IMGUIMODULE_H

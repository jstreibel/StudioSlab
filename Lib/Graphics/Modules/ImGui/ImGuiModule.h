//
// Created by joao on 22/09/23.
//

#ifndef STUDIOSLAB_IMGUIMODULE_H
#define STUDIOSLAB_IMGUIMODULE_H

#include "Core/Backend/Implementations.h"
#include "Core/Backend/Modules/Module.h"

#include "Graphics/Modules/GraphicsModule.h"

#include "3rdParty/ImGui.h"
#include "ImGuiContext.h"

namespace Slab::Graphics {

    class ImGuiModule : public GraphicsModule {
        void generalInitialization();
        void buildFonts();

        using Context = Pointer<SlabImGuiContext>;

        ImGuiContext *main_context = nullptr;
        Context m_SlabContext = nullptr;

        Vector<Context> contexts;

    protected:
        bool showDemos = false;

        void finishInitialization();
        explicit ImGuiModule();
        ~ImGuiModule() override;

    public:
        virtual
        void NewFrame();
        virtual
        void RenderFrame();
        // virtual
        // void endFrame(bool do_render=true);

        Pointer<SlabImGuiContext> createContext();

        static ImGuiModule* BuildModule();

        void beginRender() override;

        void endRender() override;

        Pointer<SlabImGuiContext> getContext();
    };

} // Core

#endif //STUDIOSLAB_IMGUIMODULE_H

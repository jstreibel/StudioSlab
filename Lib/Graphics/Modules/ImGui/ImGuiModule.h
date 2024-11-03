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

    using Context = Pointer<SlabImGuiContext>;
    using ContextInitializer = std::function<void(ImGuiContext*)>;

    class ImGuiModule : public GraphicsModule {
        static void buildFonts();

        Context m_SlabContext = nullptr;

        ContextInitializer initializeContext;

    protected:
        Vector<Context> contexts;

        bool showDemos = false;

        explicit ImGuiModule(ContextInitializer);
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

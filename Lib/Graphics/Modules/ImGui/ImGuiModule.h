//
// Created by joao on 22/09/23.
//

#ifndef STUDIOSLAB_IMGUIMODULE_H
#define STUDIOSLAB_IMGUIMODULE_H

#include "Core/Backend/Implementations.h"
#include "Core/Backend/Modules/Module.h"
#include "Graphics/Modules/GraphicsModule.h"

namespace Slab::Graphics {

    class ImGuiModule : public GraphicsModule {
        static void generalInitialization();
        static void buildFonts();

    protected:
        bool showDemos = false;

        static void finishInitialization();
        explicit ImGuiModule();
        ~ImGuiModule() override;

    public:
        static ImGuiModule* BuildModule();

        void beginRender() override;

        void endRender() override;
    };

} // Core

#endif //STUDIOSLAB_IMGUIMODULE_H

//
// Created by joao on 22/09/23.
//

#ifndef STUDIOSLAB_IMGUIMODULE_H
#define STUDIOSLAB_IMGUIMODULE_H

#include "Core/Backend/Implementations.h"
#include "Core/Backend/Modules/Module.h"

namespace Core {

    class ImGuiModule : public Module {
        static void generalInitialization();
        static void buildFonts();

        BackendImplementation system;

    protected:
        static void finishInitialization();
        explicit ImGuiModule(BackendImplementation);
        ~ImGuiModule() override;

    public:
        static ImGuiModule* BuildModule(BackendImplementation);

        void beginRender() override;

        void endRender() override;
    };

} // Core

#endif //STUDIOSLAB_IMGUIMODULE_H

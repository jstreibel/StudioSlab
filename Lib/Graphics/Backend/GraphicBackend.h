//
// Created by joao on 08/07/23.
//

#ifndef STUDIOSLAB_GRAPHICBACKEND_H
#define STUDIOSLAB_GRAPHICBACKEND_H

#include "Core/Backend/Backend.h"
#include "Graphics/Modules/GraphicsModule.h"
#include "SystemWindow.h"

namespace Slab::Graphics {

    class GraphicBackend : public Core::Backend {
        void notifyModuleLoaded(const Pointer<Slab::Core::Module> &pointer) override;

    protected:

        explicit GraphicBackend(const Str &name);

        Vector<Volatile<GraphicsModule>> graphicModules;
        List<Pointer<SystemWindow>> system_windows;

        virtual void clearModules();
        void unloadAllModules();

        virtual
        Pointer<SystemWindow> CreateSystemWindow(const Str& title) = 0;

    public:
        ~GraphicBackend() override;

        Pointer<SystemWindow>
        NewSystemWindow(const Str& title);

        Pointer<SystemWindow>
        GetMainSystemWindow();

        void SetupGUI(SystemWindow *);

        void addGraphicsModule(const Volatile<GraphicsModule> &module);

        auto getGraphicsModules() -> const Vector<Volatile<GraphicsModule>> &;

        void terminate() override;

        auto isHeadless() const -> bool final;
    };

}

#endif //STUDIOSLAB_GRAPHICBACKEND_H

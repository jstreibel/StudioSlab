//
// Created by joao on 08/07/23.
//

#ifndef STUDIOSLAB_GRAPHICBACKEND_H
#define STUDIOSLAB_GRAPHICBACKEND_H

#include "Core/Backend/Backend.h"
#include "Graphics/Modules/GraphicsModule.h"
#include "SystemWindow.h"

namespace Slab::Graphics {

    class GraphicBackend : public Core::FBackend {
        void NotifyModuleLoaded(const Pointer<Slab::Core::SlabModule> &pointer) override;

    protected:

        explicit GraphicBackend(const Str &name);

        Vector<Volatile<GraphicsModule>> graphicModules;
        List<Pointer<SystemWindow>> SystemWindows;

        virtual void ClearModules();
        void UnloadAllModules();

        virtual
        Pointer<SystemWindow> CreateSystemWindow(const Str& title) = 0;

    public:
        ~GraphicBackend() override;

        Pointer<SystemWindow>
        NewSystemWindow(const Str& title);

        Pointer<SystemWindow>
        GetMainSystemWindow();

        void SetupGUI(const SystemWindow *) const;

        void AddGraphicsModule(const Volatile<GraphicsModule> &module);

        auto GetGraphicsModules() -> const Vector<Volatile<GraphicsModule>> &;

        void Terminate() override;

        auto IsHeadless() const -> bool final;
    };

}

#endif //STUDIOSLAB_GRAPHICBACKEND_H

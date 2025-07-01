//
// Created by joao on 08/07/23.
//

#ifndef STUDIOSLAB_GRAPHICBACKEND_H
#define STUDIOSLAB_GRAPHICBACKEND_H

#include "Core/Backend/Backend.h"
#include "Graphics/Modules/GraphicsModule.h"
#include "PlatformWindow.h"

namespace Slab::Graphics {

    class GraphicBackend : public Core::FBackend {
        void NotifyModuleLoaded(const Pointer<Slab::Core::SlabModule> &pointer) override;

    protected:

        explicit GraphicBackend(const Str &name);

        Vector<TVolatile<GraphicsModule>> GraphicModules;
        List<Pointer<FPlatformWindow>> SystemWindows;

        virtual void ClearModules();
        void UnloadAllModules();

        virtual
        Pointer<FPlatformWindow> CreatePlatformWindow(const Str& title) = 0;

    public:
        ~GraphicBackend() override;

        Pointer<FPlatformWindow>
        NewSystemWindow(const Str& title);

        Pointer<FPlatformWindow>
        GetMainSystemWindow();

        void SetupGUI(const FPlatformWindow *) const;

        void AddGraphicsModule(const TVolatile<GraphicsModule> &module);

        auto GetGraphicsModules() -> const Vector<TVolatile<GraphicsModule>> &;

        void Terminate() override;

        auto IsHeadless() const -> bool final;
    };

}

#endif //STUDIOSLAB_GRAPHICBACKEND_H

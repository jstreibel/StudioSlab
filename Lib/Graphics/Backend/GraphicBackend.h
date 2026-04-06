//
// Created by joao on 08/07/23.
//

#ifndef STUDIOSLAB_GRAPHICBACKEND_H
#define STUDIOSLAB_GRAPHICBACKEND_H

#include "Core/Backend/Backend.h"
#include "Graphics/Modules/GraphicsModule.h"

namespace Slab::Graphics {

    class FGraphicBackend : public Core::FBackend {
        void NotifyModuleLoaded(const TPointer<Slab::Core::FSlabModule> &pointer) override;

    protected:

        explicit FGraphicBackend(const Str &name);

        Vector<TVolatile<FGraphicsModule>> GraphicModules;
        TList<TPointer<FPlatformWindow>> SystemWindows;

        virtual void ClearModules();
        void UnloadAllModules();

        virtual
        TPointer<FPlatformWindow> CreatePlatformWindow(const Str& title) = 0;

    public:
        ~FGraphicBackend() override;

        TPointer<FPlatformWindow>
        NewSystemWindow(const Str& title);

        TPointer<FPlatformWindow>
        GetMainSystemWindow();

        void SetupGUIForPlatformWindow(const FPlatformWindow *) const;

        void AddGraphicsModule(const TVolatile<FGraphicsModule>& module);

        auto GetGraphicsModules() -> const Vector<TVolatile<FGraphicsModule>>&;

        void Terminate() override;

        auto IsHeadless() const -> bool final;
    };

    using GraphicBackend [[deprecated("Use FGraphicBackend")]] = FGraphicBackend;

}

#endif //STUDIOSLAB_GRAPHICBACKEND_H

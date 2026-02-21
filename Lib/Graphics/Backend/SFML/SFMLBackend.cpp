//
// Created by joao on 11/4/21.
//

#include "SFMLBackend.h"
#include "Graphics/SlabGraphics.h"

#include "Utils/ReferenceIterator.h"
#include "SFMLSystemWindow.h"


namespace Slab::Graphics {


    FSFMLBackend::FSFMLBackend()
        : FGraphicBackend("SFML backend")
    {
    }

    void FSFMLBackend::Run() {

        sf::Clock timer;
        timer.restart();
        while (!SystemWindows.empty()) {
            off_sync.lock();

            // IterateReferences(graphicModules, FuncRun(beginEvents));
            for(const auto& system_window : SystemWindows)
                DynamicPointerCast<FSFMLSystemWindow>(system_window)->PollEvents();
            // IterateReferences(graphicModules, FuncRun(endEvents));

            for(const auto& system_window : SystemWindows)
                system_window->Render();

            timer.restart();

            off_sync.unlock();
        }
    }

    FSFMLBackend& FSFMLBackend::GetInstance() {
        auto guiBackend = Slab::Graphics::GetGraphicsBackend();

        return *DynamicPointerCast<FSFMLBackend>(guiBackend);
    }

    void FSFMLBackend::Terminate() {
        off_sync.lock();

        // window->close();
        ClearModules();
        SystemWindows.clear();

        off_sync.unlock();
    }

    TPointer<FPlatformWindow> FSFMLBackend::CreatePlatformWindow(const Str &title) {
        return New<FSFMLSystemWindow>();
    }

}

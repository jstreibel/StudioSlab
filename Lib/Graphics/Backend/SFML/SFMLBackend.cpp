//
// Created by joao on 11/4/21.
//

#include "SFMLBackend.h"
#include "Graphics/SlabGraphics.h"

#include "Utils/ReferenceIterator.h"
#include "SFMLSystemWindow.h"


namespace Slab::Graphics {


    SFMLBackend::SFMLBackend()
        : GraphicBackend("SFML backend")
    {
    }

    void SFMLBackend::Run() {

        sf::Clock timer;
        timer.restart();
        while (!SystemWindows.empty()) {
            off_sync.lock();

            // IterateReferences(graphicModules, FuncRun(beginEvents));
            for(const auto& system_window : SystemWindows)
                DynamicPointerCast<SFMLSystemWindow>(system_window)->PollEvents();
            // IterateReferences(graphicModules, FuncRun(endEvents));

            for(const auto& system_window : SystemWindows)
                system_window->Render();

            timer.restart();

            off_sync.unlock();
        }
    }

    SFMLBackend &SFMLBackend::GetInstance() {
        auto guiBackend = Slab::Graphics::GetGraphicsBackend();

        return *DynamicPointerCast<SFMLBackend>(guiBackend);
    }

    void SFMLBackend::Terminate() {
        off_sync.lock();

        // window->close();
        ClearModules();
        SystemWindows.clear();

        off_sync.unlock();
    }

    Pointer<FPlatformWindow> SFMLBackend::CreatePlatformWindow(const Str &title) {
        return New<SFMLSystemWindow>();
    }

}
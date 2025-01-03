//
// Created by joao on 11/4/21.
//

#include "SFMLBackend.h"
#include "Graphics/SlabGraphics.h"

#include "Utils/ReferenceIterator.h"
#include "SFMLSystemWindow.h"


namespace Slab::Graphics {


    SFMLBackend::SFMLBackend()
    : GraphicBackend("SFML backend") {    }

    void SFMLBackend::run() {

        sf::Clock timer;
        timer.restart();
        while (!system_windows.empty()) {
            off_sync.lock();

            // IterateReferences(graphicModules, FuncRun(beginEvents));
            for(const auto& system_window : system_windows)
                DynamicPointerCast<SFMLSystemWindow>(system_window)->PollEvents();
            // IterateReferences(graphicModules, FuncRun(endEvents));

            for(const auto& system_window : system_windows)
                system_window->Render();

            timer.restart();

            off_sync.unlock();
        }
    }

    SFMLBackend &SFMLBackend::GetInstance() {
        auto guiBackend = Slab::Graphics::GetGraphicsBackend();

        return *DynamicPointerCast<SFMLBackend>(guiBackend);
    }

    void SFMLBackend::terminate() {
        off_sync.lock();

        // window->close();
        clearModules();
        system_windows.clear();

        off_sync.unlock();
    }

    Pointer<SystemWindow> SFMLBackend::CreateSystemWindow(const Str &title) {
        return New<SFMLSystemWindow>();
    }

}
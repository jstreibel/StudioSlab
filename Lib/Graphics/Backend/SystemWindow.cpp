//
// Created by joao on 10/17/24.
//

#include "SystemWindow.h"
#include "StudioSlab.h"

#include "Core/SlabCore.h"
#include "3rdParty/ImGui.h"

#include "Graphics/Modules/GUIModule/GUIModule.h"
#include "Graphics/SlabGraphics.h"
#include "Core/Tools/Log.h"

#include <utility>

namespace Slab::Graphics {

    SystemWindow::SystemWindow(void *window_ptr, Pointer<FEventTranslator> EventTranslator)
    : EventTranslator(std::move(EventTranslator))
    , window_ptr(window_ptr)
    , mouse_state(New<MouseState>(this)){

        // Add event listener manually because SystemWindow::addEventListener calls the pure abstract
        // methods SystemWindow::GetWidth and SystemWindow::GetHeight, yielding an exception upon being
        // called (even implicitly) by the constructor.
        this->EventTranslator->AddGUIEventListener(mouse_state);
    }

     auto SystemWindow::AddEventListener(const Volatile<FSystemWindowEventListener> &listener) -> bool {
        if (auto ListenerRef = listener.lock())
        {
            ListenerRef->SetParentSystemWindow(this);
            // listener.lock()->notifySystemWindowReshape(getWidth(), getHeight());
        }
        else
        {
            throw Exception("Expired pointer");
        }

        return EventTranslator->AddGUIEventListener(listener);
    }

    auto SystemWindow::AddAndOwnEventListener(const Pointer<FSystemWindowEventListener> &listener) -> bool {
        if(!AddEventListener(listener)) return false;

        thingsImProprietary.push_back(listener);

        return true;
    }

    void SystemWindow::setMouseCursor(MouseCursor cursor) {
        NOT_IMPLEMENTED
        switch (cursor) {
            case Mouse_ArrowCursor:
                break;
            case Mouse_VResizeCursor:
                break;
            case Mouse_HResizeCursor:
                break;
            case Mouse_HandCursor:
                break;
            case Mouse_IBeamCursor:
                break;
            case Mouse_CrossHairCursor:
                break;
        }

        NOT_IMPLEMENTED
    }

    void SystemWindow::setSystemWindowTitle(const Str& title) {
        Core::Log::Warning() << "Could not set SystemWindow title to \"" << title << "\": Current SystemWindow implementation does allow for changing window title." << Core::Log::Flush;

    }

    void SystemWindow::clearListeners() {
        for(auto &listener : thingsImProprietary)
            listener->SetParentSystemWindow(nullptr);

        thingsImProprietary.clear();

        for(auto &listener : EventTranslator->sysWin_listeners)
            if(auto ptr = listener.lock()) ptr->SetParentSystemWindow(nullptr);

        EventTranslator->clear();
    }

    RawPaltformWindow_Ptr SystemWindow::getRawPlatformWindowPointer() {
        return window_ptr;
    }

    auto SystemWindow::getMouseState() const -> Pointer<const MouseState> {
        return mouse_state;
    }

    Volatile<GUIContext> SystemWindow::getGUIContext() {
        if(guiContext == nullptr)
            GetGraphicsBackend()->SetupGUI(this);

        return guiContext;
    }

    void SystemWindow::Render() {
        if(guiContext != nullptr) {
            static auto show_metrics = false;

            auto action = [this](const Str& item){
                if(item == "Close")        SignalClose();
                else
                if(item == "Show metrics") show_metrics = true;
            };

            guiContext->AddMainMenuItem(MainMenuItem{MainMenuLocation{"Window"},
                                                     {MainMenuLeafEntry{"Show metrics", "Alt+m", show_metrics},
                                                      MainMenuLeafEntry{"Close", "Alt+F4"}
                                                      },
                                                     action});

            if(show_metrics) guiContext->AddDrawCall([](){ ImGui::ShowMetricsWindow(&show_metrics); });
        }

        Cycle();
    }


} // Slab::Core
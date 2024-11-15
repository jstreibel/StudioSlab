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

    SystemWindow::SystemWindow(void *window_ptr, Pointer<EventTranslator> evt_translator)
    : event_translator(std::move(evt_translator))
    , window_ptr(window_ptr)
    , mouse_state(New<MouseState>(this)){

        // Add event listener manually because SystemWindow::addEventListener calls the pure abstract
        // methods SystemWindow::GetWidth and SystemWindow::GetHeight, yielding an exception upon being
        // called (even implicitly) by the constructor.
        event_translator->addGUIEventListener(mouse_state);
    }

     auto SystemWindow::addEventListener(const Volatile<SystemWindowEventListener> &listener) -> bool {
         if(listener.expired()) throw Exception("Expired pointer");

         listener.lock()->setParentSystemWindow(this);
         // listener.lock()->notifySystemWindowReshape(getWidth(), getHeight());

        return event_translator->addGUIEventListener(listener);
    }

    auto SystemWindow::addAndOwnEventListener(const Pointer<SystemWindowEventListener> &listener) -> bool {
        if(!addEventListener(listener)) return false;

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
        NOT_IMPLEMENTED
    }

    void SystemWindow::clearListeners() {
        for(auto &listener : thingsImProprietary)
            listener->setParentSystemWindow(nullptr);

        thingsImProprietary.clear();

        for(auto &listener : event_translator->syswin_listeners)
            if(auto ptr = listener.lock()) ptr->setParentSystemWindow(nullptr);

        event_translator->clear();
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
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
    , r_Window(window_ptr)
    , MouseState(New<FMouseState>(this)){

        // Add event listener manually because SystemWindow::addEventListener calls the pure abstract
        // methods SystemWindow::GetWidth and SystemWindow::GetHeight, yielding an exception upon being
        // called (even implicitly) by the constructor.
        this->EventTranslator->AddGUIEventListener(MouseState);
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

        Stash.push_back(listener);

        return true;
    }

    void SystemWindow::SetMouseCursor(MouseCursor cursor) {
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

    void SystemWindow::SetSystemWindowTitle(const Str& title) {
        Core::Log::Warning() << "Could not set SystemWindow title to \"" << title << "\": Current SystemWindow implementation does allow for changing window title." << Core::Log::Flush;

    }

    void SystemWindow::ClearListeners() {
        for(auto &listener : Stash)
            listener->SetParentSystemWindow(nullptr);

        Stash.clear();

        for(auto &listener : EventTranslator->SysWinListeners)
            if(auto ptr = listener.lock()) ptr->SetParentSystemWindow(nullptr);

        EventTranslator->clear();
    }

    RawPlatformWindow_Ptr SystemWindow::GetRawPlatformWindowPointer() const
    {
        return r_Window;
    }

    auto SystemWindow::GetMouseState() const -> Pointer<const FMouseState> {
        return MouseState;
    }

    Pointer<GUIContext> SystemWindow::GetGUIContext() {
        if(GuiContext == nullptr)
            GetGraphicsBackend()->SetupGUI(this);

        return GuiContext;
    }

    void SystemWindow::Render() {
        if(GuiContext != nullptr) {
            static auto ShowMetrics = false;

            auto action = [this](const Str& item){
                if(item == "Close")        SignalClose();
                else
                if(item == "Show metrics") ShowMetrics = true;
            };

            GuiContext->AddMainMenuItem(MainMenuItem{MainMenuLocation{"Window"},
                                                     {MainMenuLeafEntry{"Show metrics", "Alt+m", ShowMetrics},
                                                      MainMenuLeafEntry{"Close", "Alt+F4"}
                                                      },
                                                     action});

            if(ShowMetrics) GuiContext->AddDrawCall([](){ ImGui::ShowMetricsWindow(&ShowMetrics); });
        }

        Cycle();
    }


} // Slab::Core
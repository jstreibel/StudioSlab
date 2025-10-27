//
// Created by joao on 10/17/24.
//

#include "PlatformWindow.h"

#include "Core/SlabCore.h"
#include "3rdParty/ImGui.h"

#include "Graphics/Modules/GUIModule/GUIModule.h"
#include "Graphics/SlabGraphics.h"
#include "Core/Tools/Log.h"

#include <utility>

namespace Slab::Graphics {

    FPlatformWindow::FPlatformWindow(void *window_ptr, TPointer<FEventTranslator> EventTranslator)
    : EventTranslator(std::move(EventTranslator))
    , r_Window(window_ptr)
    , MouseState(New<FMouseState>())
    , KeyboardState(New<FKeyboardState>()) {
        // Add event listener manually because FPlatformWindow::AddEventListener calls the pure abstract
        // methods FPlatformWindow::GetWidth and FPlatformWindow::GetHeight, yielding an exception upon being
        // called (even implicitly) by the constructor.
        this->EventTranslator->AddGUIEventListener(MouseState);
        this->EventTranslator->AddGUIEventListener(KeyboardState);
        // AddEventListener(MouseState);
    }

     auto FPlatformWindow::AddEventListener(const TVolatile<FPlatformWindowEventListener> &Listener) const -> void {
        const auto Listener_Ptr = Listener.lock();
        if(Listener_Ptr == nullptr) throw Exception("Listener pointer is null.");

        Listener_Ptr->NotifySystemWindowReshape(GetWidth(), GetHeight());

        EventTranslator->AddGUIEventListener(Listener);
    }

    auto FPlatformWindow::AddAndOwnEventListener(const TPointer<FPlatformWindowEventListener> &Listener) -> void {
        AddEventListener(Listener);
        Stash.push_back(Listener);
    }

    void FPlatformWindow::SetMouseCursor(FMouseCursor Cursor) {
        NOT_IMPLEMENTED
        switch (Cursor) {
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

    void FPlatformWindow::SetSystemWindowTitle(const Str& title) {
        Core::Log::Warning()
        << "Could not set SystemWindow title to \""
        << title
        << "\": Current SystemWindow implementation does allow for changing window title."
        << Core::Log::Flush;
    }

    bool FPlatformWindow::ProvideSelfReference(const TVolatile<FPlatformWindow>& _SelfReference)
    {
        if (IN SelfReferencePtr = _SelfReference.lock())
        {
            if (SelfReferencePtr.get() != this)
            {
                Core::Log::Error() << "PlatformWindow was provided self-pointer to different PlatformWindow." << Core::Log::Flush;
                return false;
            }
        }

        this->SelfReference = _SelfReference;

        return true;
    }

    void FPlatformWindow::ClearListeners() {
        Stash.clear();
        EventTranslator->clear();
    }

    FPlatformWindow_RawPointer FPlatformWindow::GetRawPlatformWindowPointer() const
    {
        return r_Window;
    }

    auto FPlatformWindow::GetMouseState() const -> TPointer<const FMouseState> {
        return MouseState;
    }

    auto FPlatformWindow::GetKeyboardState() const -> TPointer<const class FKeyboardState> {
        return KeyboardState;
    }

    TPointer<FGUIContext> FPlatformWindow::GetGUIContext() const
    {
        return GuiContext;
    }

    TPointer<FGUIContext> FPlatformWindow::SetupGUIContext() {
        if(GuiContext == nullptr) {
            GetGraphicsBackend()->SetupGUIForPlatformWindow(this);

            AddEventListener(GuiContext);

            GuiContext->SetManualRender(true);
        }

        return GuiContext;
    }

    void FPlatformWindow::Render() {
        if(GuiContext != nullptr) {
            static auto ShowMetrics = false;

            auto Action = [this](const Str& Item){
                if(Item == "Close")        SignalClose();
                else
                if(Item == "Show metrics") ShowMetrics = true;
            };

            GuiContext->AddMainMenuItem(MainMenuItem{MainMenuLocation{"Window"},
                                                     {MainMenuLeafEntry{"Show metrics", "", ShowMetrics},
                                                      MainMenuLeafEntry{"Close", "Alt+F4"}
                                                      },
                                                     Action});

            if(ShowMetrics) GuiContext->AddDrawCall([](){ ImGui::ShowMetricsWindow(&ShowMetrics); });

            GuiContext->Bind();
            GuiContext->NewFrame();
        }

        Tick();

        if (GuiContext != nullptr) GuiContext->Render();

        Flush();
    }


} // Slab::Core

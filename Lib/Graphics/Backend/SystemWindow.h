//
// Created by joao on 10/17/24.
//

#ifndef STUDIOSLAB_SYSTEMWINDOW_H
#define STUDIOSLAB_SYSTEMWINDOW_H

#include "Utils/Numbers.h"
#include "Utils/String.h"
#include "Utils/Arrays.h"

#include "Graphics/Backend/Events/SystemWindowEventListener.h"
#include "Graphics/Backend/Events/MouseState.h"
#include "Graphics/Backend/Events/SystemWindowEventTranslator.h"
#include "Graphics/Modules/GUIModule/GUIContext.h"

namespace Slab::Graphics {

    using RawPaltformWindow_Ptr = void*;

    class SystemWindow {
        friend class GraphicBackend;

        Vector<Pointer<FSystemWindowEventListener>> thingsImProprietary;
        Pointer<GUIContext> guiContext = nullptr;

    protected:
        Pointer<FEventTranslator> EventTranslator;
        RawPaltformWindow_Ptr window_ptr;

        virtual void clearListeners();

        Pointer<MouseState> mouse_state = nullptr;

        virtual void Cycle() = 0;
    public:
        explicit SystemWindow(void *window_ptr, Pointer<FEventTranslator>);

        virtual ~SystemWindow() = default;

        void Render();

        [[nodiscard]] virtual Int getHeight() const = 0;
        [[nodiscard]] virtual Int getWidth() const = 0;

        virtual void SignalClose() = 0;
        [[nodiscard]] virtual bool ShouldClose() const = 0;

        Volatile<GUIContext> getGUIContext();

        RawPaltformWindow_Ptr getRawPlatformWindowPointer();

        auto AddEventListener(const Volatile<FSystemWindowEventListener> &listener) -> bool;
        auto AddAndOwnEventListener(const Pointer<FSystemWindowEventListener> &listener) -> bool;

        [[nodiscard]] auto getMouseState() const -> Pointer<const MouseState>;
        virtual void setMouseCursor(MouseCursor);

        virtual
        void setSystemWindowTitle(const Str& title);
    };

} // Slab::Core

#endif //STUDIOSLAB_SYSTEMWINDOW_H

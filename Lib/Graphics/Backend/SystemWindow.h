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

namespace Slab::Graphics {

    class SystemWindow {
        Vector<Pointer<SystemWindowEventListener>> thingsImProprietary;

    protected:
        Pointer<EventTranslator> event_translator;
        void* window_ptr;

        virtual void clearListeners();

        Pointer<MouseState> mouse_state = nullptr;

    public:
        explicit SystemWindow(void *window_ptr, Pointer<EventTranslator>);
        ~SystemWindow() = default;

        virtual Int getHeight() const = 0;
        virtual Int getWidth() const = 0;
        virtual void Render() = 0;
        virtual bool ShouldClose() const = 0;

        void* getRawPlatformWindowPointer();

        auto addEventListener(const Volatile<SystemWindowEventListener> &listener) -> bool;
        auto addAndOwnEventListener(const Pointer<SystemWindowEventListener> &listener) -> bool;

        auto getMouseState() const -> Pointer<const MouseState>;
        virtual void setMouseCursor(MouseCursor);

        void setSystemWindowTitle(Str title);
    };

} // Slab::Core

#endif //STUDIOSLAB_SYSTEMWINDOW_H

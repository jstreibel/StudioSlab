//
// Created by joao on 10/17/24.
//

#include "SystemWindow.h"

namespace Slab::Graphics {

    SystemWindow::SystemWindow(void *window_ptr, Pointer<EventTranslator> evt_translator)
    : event_translator(evt_translator)
    , window_ptr(window_ptr)
    , mouse_state(New<MouseState>()){

        // Add event listener manually because SystemWindow::addEventListener calls the pure abstract
        // methods SystemWindow::GetWidth and SystemWindow::GetHeight, yielding an exception upon being
        // called (even implicitly) by the constructor.
        event_translator->addGUIEventListener(mouse_state);
    }

     auto SystemWindow::addEventListener(const Volatile<SystemWindowEventListener> &listener) -> bool {
         if(listener.expired()) throw Exception("Expired pointer");

         listener.lock()->notifySystemWindowReshape(getWidth(), getHeight());

        return event_translator->addGUIEventListener(listener);
    }

    auto SystemWindow::addAndOwnEventListener(const Pointer<SystemWindowEventListener> &listener) -> bool {
        if(!addEventListener(listener)) return false;

        thingsImProprietary.push_back(listener);

        return true;
    }

    void SystemWindow::setMouseCursor(MouseCursor cursor) {

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

    void SystemWindow::setSystemWindowTitle(Str title, int handle) {

    }

    void SystemWindow::setSystemWindowTitle(Str title) {

    }

    void SystemWindow::clearListeners() {
        thingsImProprietary.clear();
    }

    void *SystemWindow::getRawPlatformWindowPointer() {
        return window_ptr;
    }

    auto SystemWindow::getMouseState() const -> Pointer<const MouseState> {
        return mouse_state;
    }


} // Slab::Core
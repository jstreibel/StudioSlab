//
// Created by joao on 08/07/23.
//

#include "GUIBackend.h"

GUIBackend::GUIBackend(Str name) : Backend(name) {  }

auto GUIBackend::getMouseState() const -> const MouseState & { return mouseState; }

auto GUIBackend::addWindow(Window::Ptr window) -> void {
    windows.emplace_back(window);
}

GUIBackend &GUIBackend::GetInstance() {
    return DerivableSingleton::GetInstance<GUIBackend>();
}

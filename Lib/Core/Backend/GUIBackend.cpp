//
// Created by joao on 08/07/23.
//

#include "GUIBackend.h"

GUIBackend::GUIBackend(Str name) : Backend(name) {  }

auto GUIBackend::getMouseState() const -> const MouseState & { return mouseState; }

auto GUIBackend::addEventListener(Core::GUIEventListener::Ptr listener) -> void {
    listeners.emplace_back(listener);
}

void GUIBackend::addModule(const std::shared_ptr<Core::Module> &module) { modules.emplace_back(module); }

const std::vector<std::shared_ptr<Core::Module>> &GUIBackend::getModules() { return modules; }

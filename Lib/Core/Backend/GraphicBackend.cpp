//
// Created by joao on 08/07/23.
//

#include "GraphicBackend.h"

GraphicBackend::GraphicBackend(const Str& name, Core::EventTranslator &eventTranslator)
: Backend(name), eventTranslator(eventTranslator)
{  }

auto GraphicBackend::addEventListener(const Core::GUIEventListener::Ptr &listener) -> bool {
    return eventTranslator.addGUIEventListener(listener);
}

void GraphicBackend::addModule(const std::shared_ptr<Core::Module> &module) {
    modules.emplace_back(module);
}

const std::vector<std::shared_ptr<Core::Module>> &GraphicBackend::getModules() {
    return modules;
}

void GraphicBackend::setClearColor(Real _r, Real _g, Real _b) {
    r = _r;
    g = _g;
    b = _b;
}

bool GraphicBackend::isHeadless() const {    return false;   }

//
// Created by joao on 08/07/23.
//

#include "GraphicBackend.h"

#include <utility>
#include "BackendManager.h"
#include "Core/Backend/Modules/TaskManager/TaskManager.h"

namespace Slab::Core {

    GraphicBackend::GraphicBackend(const Str &name, Pointer<EventTranslator> eventTranslator)
    : Backend(name), eventTranslator(std::move(eventTranslator)) {}

    GraphicBackend::~GraphicBackend() {
    };

    auto GraphicBackend::addEventListener(const Volatile<GUIEventListener> &listener) -> bool {
        return eventTranslator->addGUIEventListener(listener);
    }

    void GraphicBackend::addGraphicsModule(const Volatile<GraphicsModule> &module) {
        graphicModules.emplace_back(module);
    }

    const Vector<Volatile<GraphicsModule>> &GraphicBackend::getGraphicsModules() {
        return graphicModules;
    }

    void GraphicBackend::setClearColor(Real _r, Real _g, Real _b) {
        r = _r;
        g = _g;
        b = _b;
    }

    bool GraphicBackend::isHeadless() const { return false; }

    void GraphicBackend::setMouseCursor(MouseCursor cursor) {
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

    void GraphicBackend::unloadAllModules() {
        graphicModules.clear();
    }

    void GraphicBackend::setSystemWindowTitle(Str title, int handle) { NOT_IMPLEMENTED_CLASS_METHOD }


}
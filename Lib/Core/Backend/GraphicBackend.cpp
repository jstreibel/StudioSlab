//
// Created by joao on 08/07/23.
//

#include "GraphicBackend.h"
#include "BackendManager.h"
#include "Core/Backend/Modules/TaskManager/TaskManager.h"

namespace Slab::Core {

    GraphicBackend::GraphicBackend(const Str &name, EventTranslator &eventTranslator)
            : Backend(name), eventTranslator(eventTranslator) {}

    auto GraphicBackend::addEventListener(const GUIEventListener_ptr &listener) -> bool {
        return eventTranslator.addGUIEventListener(listener);
    }

    void GraphicBackend::addGraphicsModule(const Pointer<GraphicsModule> &module) {
        graphicModules.emplace_back(module);
    }

    const Vector<Pointer<GraphicsModule>> &GraphicBackend::getGraphicsModules() {
        return graphicModules;
    }

    void GraphicBackend::setClearColor(Real _r, Real _g, Real _b) {
        r = _r;
        g = _g;
        b = _b;
    }

    bool GraphicBackend::isHeadless() const { return false; }

    GraphicBackend::~GraphicBackend() = default;

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


}
//
// Created by joao on 08/07/23.
//

#include "GraphicBackend.h"

#include <utility>
#include "Core/Backend/BackendManager.h"
#include "Core/Backend/Modules/TaskManager/TaskManager.h"

namespace Slab::Graphics {

    GraphicBackend::GraphicBackend(const Str &name, Pointer<EventTranslator> eventTranslator)
    : Backend(name), eventTranslator(std::move(eventTranslator)) {}

    GraphicBackend::~GraphicBackend() = default;

    auto GraphicBackend::addEventListener(const Volatile<GUIEventListener> &listener) -> bool {
        return eventTranslator->addGUIEventListener(listener);
    }

    auto GraphicBackend::addAndOwnEventListener(const Pointer<GUIEventListener> &listener) -> bool {
        if(!addEventListener(listener)) return false;

        thingsImProprietary.push_back(listener);

        return true;
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
    void GraphicBackend::setSystemWindowTitle(Str title) { setSystemWindowTitle(title, 0); }


    void GraphicBackend::clearModules() {
        graphicModules.clear();
    }

    void GraphicBackend::clearListeners() {
        thingsImProprietary.clear();
    }

    void GraphicBackend::terminate() {
        unloadAllModules();
        clearListeners();
    }

    void GraphicBackend::notifyModuleLoaded(const Pointer<Core::Module> &module) {
        if(module->requiresGraphicsBackend) {
            auto graphic_module = DynamicPointerCast<GraphicsModule>(module);

            addGraphicsModule(graphic_module);
        }
    }


}
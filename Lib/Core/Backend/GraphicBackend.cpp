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

    void GraphicBackend::addModule(const std::shared_ptr<Module> &module) {
        modules.emplace_back(module);
    }

    const Vector<std::shared_ptr<Module>> &GraphicBackend::getModules() {
        return modules;
    }

    void GraphicBackend::setClearColor(Real _r, Real _g, Real _b) {
        r = _r;
        g = _g;
        b = _b;
    }

    bool GraphicBackend::isHeadless() const { return false; }

    GraphicBackend::~GraphicBackend() {
        /*
        auto mod = BackendManager::GetModule(Modules::TaskManager);

        auto taskManager = dynamic_cast<TaskManagerModule*>(mod.get());

        taskManager->signalFinishAllTasks();
         */
    }

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


}
//
// Created by joao on 08/07/23.
//

#ifndef STUDIOSLAB_GRAPHICBACKEND_H
#define STUDIOSLAB_GRAPHICBACKEND_H

#include "Core/Backend/Backend.h"
#include "Core/Backend/Events/MouseState.h"
#include "Core/Backend/Events/GUIEventTranslator.h"

class GraphicBackend : public Core::Backend {
    Core::EventTranslator &eventTranslator;

protected:
    GraphicBackend(const Str& name, Core::EventTranslator &eventTranslator);

    std::vector<std::shared_ptr<Core::Module>> modules;

    Real r=0, g=0, b=0;
public:
    virtual auto addEventListener(const Core::GUIEventListener_ptr &listener) -> bool;

    virtual auto getScreenHeight() const -> Real = 0;

    virtual auto getMouseState() const -> MouseState = 0;
    virtual auto pause()  -> void = 0;
    virtual auto resume() -> void = 0;

    void setClearColor(Real r, Real g, Real b);

    void addModule(const std::shared_ptr<Core::Module>& module);
    auto getModules() -> const std::vector<std::shared_ptr<Core::Module>>&;

    virtual auto requestRender() -> void = 0;

    auto isHeadless() const -> bool final;
};


#endif //STUDIOSLAB_GRAPHICBACKEND_H

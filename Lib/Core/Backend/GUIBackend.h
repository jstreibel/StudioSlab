//
// Created by joao on 08/07/23.
//

#ifndef STUDIOSLAB_GUIBACKEND_H
#define STUDIOSLAB_GUIBACKEND_H

#include "Backend.h"
#include "Core/Graphics/Window/Window.h"
#include "Events/MouseState.h"

class GUIBackend : public Core::Backend {
protected:

    explicit GUIBackend(const Str& name);

    std::vector<Core::GUIEventListener::Ptr> listeners;
    std::vector<std::shared_ptr<Core::Module>> modules;

public:
    virtual auto addEventListener(const Core::GUIEventListener::Ptr &listener) -> void;

    virtual auto getScreenHeight() const -> Real = 0;

    virtual auto getMouseState() const -> const MouseState = 0;
    virtual auto pause()  -> void = 0;
    virtual auto resume() -> void = 0;

    void addModule(const std::shared_ptr<Core::Module>& module);
    auto getModules() -> const std::vector<std::shared_ptr<Core::Module>>&;

    virtual auto requestRender() -> void = 0;
};


#endif //STUDIOSLAB_GUIBACKEND_H

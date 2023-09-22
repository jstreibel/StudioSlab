//
// Created by joao on 08/07/23.
//

#ifndef STUDIOSLAB_GUIBACKEND_H
#define STUDIOSLAB_GUIBACKEND_H

#include "Backend.h"
#include "Core/Graphics/Window/Window.h"

class GUIBackend : public Backend {
protected:

    explicit GUIBackend(Str name);

    MouseState mouseState;
    std::vector<Window::Ptr> windows;

    bool mustRender = true;
    bool renderingRequested();

public:
    virtual auto addWindow(Window::Ptr window) -> void;

    virtual auto getScreenHeight() const -> Real = 0;

    auto getMouseState() const -> const MouseState&;
    virtual auto pause()  -> void = 0;
    virtual auto resume() -> void = 0;

    auto requestRender() -> void;

    static GUIBackend& GetInstance();

};


#endif //STUDIOSLAB_GUIBACKEND_H

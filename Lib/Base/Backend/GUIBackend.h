//
// Created by joao on 08/07/23.
//

#ifndef STUDIOSLAB_GUIBACKEND_H
#define STUDIOSLAB_GUIBACKEND_H

#include "Backend.h"
#include "Base/Graphics/Window/Window.h"

class GUIBackend : public Backend {
protected:

    GUIBackend(Str name);

    MouseState mouseState;
    std::vector<Window::Ptr> windows;

public:
    virtual auto addWindow(Window::Ptr window) -> void;

    auto getMouseState() const -> const MouseState&;
    virtual auto pause() -> void = 0;
    virtual auto resume() -> void = 0;

    static GUIBackend& GetInstance();

};


#endif //STUDIOSLAB_GUIBACKEND_H

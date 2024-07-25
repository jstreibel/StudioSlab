//
// Created by joao on 08/07/23.
//

#ifndef STUDIOSLAB_GRAPHICBACKEND_H
#define STUDIOSLAB_GRAPHICBACKEND_H

#include "Core/Backend/Backend.h"
#include "Core/Backend/Events/MouseState.h"
#include "Core/Backend/Events/GUIEventTranslator.h"

namespace Slab::Core {

    class GraphicBackend : public Backend {
    protected:
        Pointer<EventTranslator> eventTranslator;
        GraphicBackend(const Str &name, Pointer<EventTranslator> eventTranslator);

    public:
        ~GraphicBackend() override;

    protected:

        Vector<Volatile<GraphicsModule>> graphicModules;

        Real r = 0, g = 0, b = 0;
    public:
        virtual auto addEventListener(const Volatile<GUIEventListener> &listener) -> bool;

        virtual auto getScreenHeight() const -> Real = 0;
        virtual auto getMouseState() const -> MouseState = 0;
        virtual void setMouseCursor(MouseCursor);
        virtual void setSystemWindowTitle(Str title, int handle=0);

        void setClearColor(Real r, Real g, Real b);

        void addGraphicsModule(const Volatile<GraphicsModule> &module);

        auto getGraphicsModules() -> const Vector<Volatile<GraphicsModule>> &;

        void unloadAllModules();

        auto isHeadless() const -> bool final;
    };

}

#endif //STUDIOSLAB_GRAPHICBACKEND_H

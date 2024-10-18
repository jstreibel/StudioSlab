//
// Created by joao on 08/07/23.
//

#ifndef STUDIOSLAB_GRAPHICBACKEND_H
#define STUDIOSLAB_GRAPHICBACKEND_H

#include "Core/Backend/Backend.h"
#include "Graphics/Backend/Events/MouseState.h"
#include "Graphics/Backend/Events/GUIEventTranslator.h"

namespace Slab::Core {

    class GraphicBackend : public Backend {
        void notifyModuleLoaded(const Pointer<Slab::Core::Module> &pointer) override;

    protected:
        Vector<Pointer<GUIEventListener>> thingsImProprietary;
        Pointer<EventTranslator> eventTranslator;
        GraphicBackend(const Str &name, Pointer<EventTranslator> eventTranslator);

        Vector<Volatile<GraphicsModule>> graphicModules;

        Pointer<EventTranslator>
        getEventTranslator() { return eventTranslator; };

        virtual void clearModules();
        virtual void clearListeners();

        Real r = 0, g = 0, b = 0;

        void unloadAllModules();
    public:
        ~GraphicBackend() override;

        virtual auto addEventListener(const Volatile<GUIEventListener> &listener) -> bool;
        virtual auto addAndOwnEventListener(const Pointer<GUIEventListener> &listener) -> bool;

        virtual auto getScreenHeight() const -> Real = 0;
        virtual auto getMouseState() const -> MouseState = 0;
        virtual void setMouseCursor(MouseCursor);
        virtual void setSystemWindowTitle(Str title, int handle=0);

        void setClearColor(Real r, Real g, Real b);

        void addGraphicsModule(const Volatile<GraphicsModule> &module);

        auto getGraphicsModules() -> const Vector<Volatile<GraphicsModule>> &;

        void terminate() override;

        auto isHeadless() const -> bool final;
    };

}

#endif //STUDIOSLAB_GRAPHICBACKEND_H

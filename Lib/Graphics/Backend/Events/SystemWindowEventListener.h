//
// Created by joao on 02/09/2021.
//

#ifndef V_SHAPE_OPENGLARTISTBASE_H
#define V_SHAPE_OPENGLARTISTBASE_H

#include "Utils/Types.h"
#include "Utils/Utils.h"
#include "Utils/Timer.h"

#include "KeyMap.h"


namespace Slab::Graphics {

    class SystemWindow;

    using ParentSystemWindow = SystemWindow*;

    class FSystemWindowEventListener {
        Vector<Volatile<FSystemWindowEventListener>> delegateResponders;
        friend class SystemWindow;
        void setParentSystemWindow(SystemWindow *);

    protected:
        SystemWindow* parent_system_window = nullptr;

        void addResponder(const Volatile<FSystemWindowEventListener>& responder);
        void removeResponder(const Pointer<FSystemWindowEventListener>& responder);
        [[nodiscard]] bool hasResponders() const;

    public:

        explicit FSystemWindowEventListener(ParentSystemWindow parent);
        explicit FSystemWindowEventListener();
        virtual ~FSystemWindowEventListener();

        virtual bool NotifyKeyboard(KeyMap key, KeyState state, ModKeys modKeys);
        virtual bool notifyCharacter(UInt codepoint);

        virtual void cursorEntered(bool);
        virtual bool NotifyMouseButton(MouseButton, KeyState, ModKeys);
        virtual bool notifyMouseMotion(int x, int y, int dx, int dy);
        virtual bool notifyMouseWheel(double dx, double dy);

        virtual bool notifyFilesDropped(StrVector paths);

        virtual bool notifySystemWindowReshape(int w, int h);

        virtual bool notifyRender();

    };

    DefinePointers(FSystemWindowEventListener)
}


#endif //V_SHAPE_OPENGLARTISTBASE_H

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

    using FOwnerSystemWindow = SystemWindow*;

    class FSystemWindowEventListener {
        Vector<Volatile<FSystemWindowEventListener>> delegateResponders;
        friend class SystemWindow;
        void SetParentSystemWindow(SystemWindow *);

    protected:
        SystemWindow* parent_system_window = nullptr;

        void AddResponder(const Volatile<FSystemWindowEventListener>& responder);
        void RemoveResponder(const Pointer<FSystemWindowEventListener>& responder);
        [[nodiscard]] bool HasResponders() const;

    public:

        explicit FSystemWindowEventListener(FOwnerSystemWindow parent);
        explicit FSystemWindowEventListener();
        virtual ~FSystemWindowEventListener();

        virtual bool NotifyKeyboard(KeyMap key, KeyState state, ModKeys modKeys);
        virtual bool NotifyCharacter(UInt codepoint);

        virtual void CursorEntered(bool);
        virtual bool NotifyMouseButton(MouseButton, KeyState, ModKeys);
        virtual bool NotifyMouseMotion(int x, int y, int dx, int dy);
        virtual bool NotifyMouseWheel(double dx, double dy);

        virtual bool NotifyFilesDropped(StrVector paths);

        virtual bool NotifySystemWindowReshape(int w, int h);

        virtual bool NotifyRender();

    };

    DefinePointers(FSystemWindowEventListener)
}


#endif //V_SHAPE_OPENGLARTISTBASE_H

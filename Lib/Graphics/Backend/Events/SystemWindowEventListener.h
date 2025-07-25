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

    class FPlatformWindow;

    using FOwnerPlatformWindow = TVolatile<FPlatformWindow>;

    class FPlatformWindowEventListener {
        Vector<TVolatile<FPlatformWindowEventListener>> delegateResponders;
        friend class FPlatformWindow;

        int Priority;

    protected:

        void AddResponder(const TVolatile<FPlatformWindowEventListener>& responder);
        void RemoveResponder(const TPointer<FPlatformWindowEventListener>& responder);
        [[nodiscard]] bool HasResponders() const;

    public:

        explicit FPlatformWindowEventListener();
        virtual ~FPlatformWindowEventListener();

        virtual bool NotifyKeyboard(EKeyMap key, EKeyState state, EModKeys modKeys);
        virtual bool NotifyCharacter(UInt codepoint);

        virtual void CursorEntered(bool);
        virtual bool NotifyMouseButton(EMouseButton, EKeyState, EModKeys);
        virtual bool NotifyMouseMotion(int x, int y, int dx, int dy);
        virtual bool NotifyMouseWheel(double dx, double dy);

        virtual bool NotifyFilesDropped(StrVector paths);

        virtual bool NotifySystemWindowReshape(int w, int h);

        virtual bool NotifyRender(const FPlatformWindow&);

    };

    DefinePointers(FPlatformWindowEventListener)
}


#endif //V_SHAPE_OPENGLARTISTBASE_H

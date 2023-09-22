//
// Created by joao on 02/09/2021.
//

#ifndef V_SHAPE_OPENGLARTISTBASE_H
#define V_SHAPE_OPENGLARTISTBASE_H


#include "Core/Graphics/Animation.h"

#include "Utils/Types.h"
#include "Utils/Utils.h"
#include "Utils/Timer.h"

#include "KeyMap.h"


namespace Core {

    class GUIEventListener {
        std::vector<GUIEventListener*> unmanagedDelegateResponders;

    protected:
        void addResponder(GUIEventListener *responder);
        bool hasResponders() const;

    public:
        GUIEventListener();
        virtual ~GUIEventListener() = default;

        virtual bool notifyKeyboard(KeyMap key, KeyState state, ModKeys modKeys);

        virtual bool notifyMouseButton(MouseButton, KeyState, ModKeys);
        virtual bool notifyMouseMotion(int x, int y);
        virtual bool notifyMouseWheel(double dx, double dy);

        virtual bool notifyFilesDropped(StrVector paths);

        virtual bool notifyScreenReshape(int newScreenWidth, int newScreenHeight);

        virtual bool notifyRender(float elTime_msec);




        virtual bool notifyKeyboard(unsigned char key, int x, int y);
        virtual bool notifyKeyboardSpecial(int key, int x, int y);

        virtual bool notifyMouseButton(int button, int dir, int x, int y);
        virtual bool notifyMousePassiveMotion(int x, int y);
        virtual bool notifyMouseWheel(int wheel, int direction, int x, int y);

    };
}


#endif //V_SHAPE_OPENGLARTISTBASE_H

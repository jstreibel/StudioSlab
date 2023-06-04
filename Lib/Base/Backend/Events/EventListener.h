//
// Created by joao on 02/09/2021.
//

#ifndef V_SHAPE_OPENGLARTISTBASE_H
#define V_SHAPE_OPENGLARTISTBASE_H


#include "Base/Graphics/Animation.h"

#include "Common/Types.h"
#include "Common/Utils.h"
#include "Common/Timer.h"


namespace Base {

    class EventListener {
        std::vector<EventListener*> unmanagedDelegateResponders;

    protected:
        const bool allowsDelegateResponders;
        void addResponder(EventListener *responder);
        bool hasResponders() const;

    public:
        EventListener(bool allowDelegateResponders = false);;
        virtual ~EventListener() = default;

        virtual bool notifyKeyboard(unsigned char key, int x, int y);
        virtual bool notifyKeyboardSpecial(int key, int x, int y);

        virtual bool notifyMouseButton(int button, int dir, int x, int y);
        virtual bool notifyMouseMotion(int x, int y);
        virtual bool notifyMousePassiveMotion(int x, int y);
        virtual bool notifyMouseWheel(int wheel, int direction, int x, int y);

        virtual bool notifyScreenReshape(int newScreenWidth, int newScreenHeight);

        virtual bool notifyRender();
    };
}


#endif //V_SHAPE_OPENGLARTISTBASE_H

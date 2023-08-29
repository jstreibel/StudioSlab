//
// Created by joao on 02/09/2021.
//

#ifndef V_SHAPE_OPENGLARTISTBASE_H
#define V_SHAPE_OPENGLARTISTBASE_H


#include "Core/Graphics/Animation.h"

#include "Utils/Types.h"
#include "Utils/Utils.h"
#include "Utils/Timer.h"


namespace Core {

    class EventListener {
        std::vector<EventListener*> unmanagedDelegateResponders;

    protected:
        void addResponder(EventListener *responder);
        bool hasResponders() const;

    public:
        EventListener();
        virtual ~EventListener() = default;

        virtual bool notifyKeyboard(unsigned char key, int x, int y);
        virtual bool notifyKeyboardSpecial(int key, int x, int y);

        virtual bool notifyMouseButton(int button, int dir, int x, int y);
        virtual bool notifyMouseMotion(int x, int y);
        virtual bool notifyMousePassiveMotion(int x, int y);
        virtual bool notifyMouseWheel(int wheel, int direction, int x, int y);

        virtual bool notifyScreenReshape(int newScreenWidth, int newScreenHeight);

        virtual bool notifyRender(float elTime_msec);
    };
}


#endif //V_SHAPE_OPENGLARTISTBASE_H

//
// Created by joao on 02/09/2021.
//

#ifndef V_SHAPE_OPENGLARTISTBASE_H
#define V_SHAPE_OPENGLARTISTBASE_H


#include <Base/Graphics/Animation.h>

#include <Common/Typedefs.h>
#include <Common/Utils.h>
#include <Common/Timer.h>


namespace Base {

    class GLUTEventListener {
    public:
        GLUTEventListener() = default;
        virtual ~GLUTEventListener() = default;

        virtual void notifyKeyboard(unsigned char key, int x, int y)          {};
        virtual void notifyKeyboardSpecial(int key, int x, int y)             {};
        virtual void notifyMouseButton(int button, int dir, int x, int y)     {};
        virtual void notifyMouseMotion(int x, int y)                          {};
        virtual void notifyMouseWheel(int wheel, int direction, int x, int y) {};

        virtual void notifyReshape(int width, int height) {};

        virtual void notifyRender() {/*finishFrameAndRender();*/};

    protected:
        //std::vector<String> stats;

    };
}


#endif //V_SHAPE_OPENGLARTISTBASE_H

//
// Created by joao on 27/09/23.
//

#ifndef STUDIOSLAB_GLUTLISTENER_H
#define STUDIOSLAB_GLUTLISTENER_H

namespace Slab::Core {

    class GLUTListener {
    public:
        virtual bool keyboard(unsigned char key, int x, int y);
        virtual bool keyboardUp(unsigned char key, int x, int y);
        virtual bool keyboardSpecial(int key, int x, int y);
        virtual bool keyboardSpecialUp(int key, int x, int y);

        virtual bool mouseButton(int button, int dir, int x, int y);
        virtual bool mousePassiveMotion(int x, int y);
        virtual bool mouseWheel(int wheel, int dir, int x, int y);
        virtual bool mouseMotion(int x, int y);

        virtual bool render();
        virtual bool idle();
        virtual bool reshape(int w, int h);
    };

} // Backend

#endif //STUDIOSLAB_GLUTLISTENER_H

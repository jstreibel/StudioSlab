//
// Created by joao on 27/09/23.
//

#include "GLUTListener.h"

namespace Backend {

    bool GLUTListener::keyboard(unsigned char key, int x, int y)      { return false; }
    bool GLUTListener::keyboardUp(unsigned char key, int x, int y)    { return false; }
    bool GLUTListener::keyboardSpecial(int key, int x, int y)         { return false; }
    bool GLUTListener::keyboardSpecialUp(int key, int x, int y)       { return false; }
    bool GLUTListener::mouseButton(int button, int dir, int x, int y) { return false; }
    bool GLUTListener::mousePassiveMotion(int x, int y)               { return false; }
    bool GLUTListener::mouseWheel(int wheel, int dir, int x, int y)   { return false; }
    bool GLUTListener::mouseMotion(int x, int y)                      { return false; }
    bool GLUTListener::render()                                       { return false; }
    bool GLUTListener::idle()                                         { return false; }
    bool GLUTListener::reshape(int w, int h)                          { return false; }


} // Backend

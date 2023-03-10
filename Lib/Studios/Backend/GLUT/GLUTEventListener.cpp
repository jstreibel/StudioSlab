//
// Created by joao on 02/09/2021.
//

#include "GLUTEventListener.h"
#include "GLUTUtils.h"

#include <GL/gl.h>

#include <Common/Typedefs.h>



Base::GLUTEventListener::GLUTEventListener() {

}



auto Base::GLUTEventListener::getWindowSizeHint() -> IntPair {
    return {-1,-1};
}


auto Base::GLUTEventListener::needDraw() const -> bool {return true;}



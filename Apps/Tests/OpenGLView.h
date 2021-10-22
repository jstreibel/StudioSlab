//
// Created by joao on 30/08/2021.
//

#ifndef V_SHAPE_OPENGLVIEW_H
#define V_SHAPE_OPENGLVIEW_H


#include <View/Graphic/OutputOpenGL.h>

class OpenGLView : public Base::OutputOpenGL {
    Graph fieldsGraph;
public:
    OpenGLView();
    void draw() override;

};


#endif //V_SHAPE_OPENGLVIEW_H

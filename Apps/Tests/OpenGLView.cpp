//
// Created by joao on 30/08/2021.
//

#include "OpenGLView.h"
#include "View/Graphic/Tools/FunctionRenderer.h"


OpenGLView::OpenGLView() {
    fieldsGraph = {0, 0, 1024, 800, -1, 1, -0.5, 0.5};
}


void OpenGLView::draw() {
    // *************************** FIELD ***********************************
    fieldsGraph.DrawAxes();
    const RtoR::FieldState &fieldState = *lastInfo.getFieldData<RtoR::FieldState>();
    if(&fieldState == nullptr) throw "Fieldstate data doesn't seem to be RtoRMap.";

    const Color colorPhi = Color(.5f, .5f, 1.0f);

    std::cout << ".,";

    //RtoR::FunctionRenderer::renderFunction(fieldState.getPhi(), colorPhi, true);
}


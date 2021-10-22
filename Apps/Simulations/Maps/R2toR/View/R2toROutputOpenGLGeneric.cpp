//

#include "R2toROutputOpenGLGeneric.h"
#include "Apps/Simulations/Maps/R2toR/Model/Transform.h"
#include "Apps/Simulations/Maps/R2toR/Model/FunctionsCollection/AnalyticShockwave2DRadialSymmetry.h"
#include "Apps/Simulations/Maps/R2toR/Model/FunctionsCollection/FunctionAzimuthalSymmetry.h"
#include "Apps/Simulations/Maps/R2toR/Model/FieldState.h"

#include "View/Graphic/Tools/FunctionRenderer.h"

#if USE_VTK
//
// Created by joao on 23/09/2019.
#include "vtk-8.2/vtkConeSource.h"
#include "vtk-8.2/vtkPolyDataMapper.h"
#include "vtk-8.2/vtkCamera.h"
#include "vtk-8.2/vtkActor.h"


#endif

R2toR::OutputOpenGL::OutputOpenGL(Real xMin, Real xMax, Real yMin, Real yMax, Real phiMin, Real phiMax)
                                  : Base::OutputOpenGL(), xMin(xMin), xMax(xMax), yMin(yMin), yMax(yMax), phiMin(phiMin), phiMax(phiMax)
{
#if USE_VTK
    // Creation of a simple VTK pipeline
    vtkNew<vtkConeSource> cone;

    vtkNew<vtkPolyDataMapper> coneMapper;
    coneMapper->SetInputConnection( cone->GetOutputPort() );

    vtkNew<vtkActor> coneActor;
    coneActor->SetMapper( coneMapper.GetPointer() );

    renWin->AddRenderer( ren.GetPointer() );
    // Here is the trick: we ask the RenderWindow to join the current OpenGL context created by GLUT
    renWin->InitializeFromCurrentContext();

    ren->AddActor( coneActor.GetPointer() );
#endif

    std::cout << "Initialized R2toRMap::OutputOpenGL." << std::endl;
}

void R2toR::OutputOpenGL::draw() {
    if(!lastInfo.hasValidData()) return;

    const Real t0 = 0;
    const Real t = lastInfo.getT();
    const Real L = Allocator::getInstance().getNumericParams().getL();
    const Real coneRadius = t0-t;

    const R2toR::FieldState& fState = *lastInfo.getFieldData<R2toR::FieldState>();
    auto &phi = fState.getPhi();
    auto &dPhi = fState.getDPhiDt();

    glDisable(GL_LINE_STIPPLE);


    // *************************************************************************************************
    // ********** SECOES *******************************************************************************

    // *************************************************************************************************
    // ********** CAMPO INTEIRO ************************************************************************
    //glEnable(GL_DEPTH_TEST);
    phiGraph.BindWindow();
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(zpr.getModelview());

    //RtoRMap::FunctionRenderer::renderFunction(phi, xMin, xMin, xMax - xMin, phi.getN());
    RtoR::FunctionRenderer::renderFunction(phi, xMin, xMin, xMax - xMin, 100, 100);
    //zpr.popModelViewMatrix();
    // Secoes sobre o campo inteiro

    /*
    glEnable(GL_LINE_STIPPLE);
    glLineStipple(1, 0xAAAA);
    glColor4d(.0, .0, .0, .5);
    glLineWidth(5);
    glBegin(GL_LINES);
    {
        glVertex2d(-2, 0);
        glVertex2d(2, 0);
    }
    glEnd();

    glBegin(GL_POINTS);
    {
        glVertex2d(.0, .0);
    }
    glEnd();
    glDisable(GL_LINE_STIPPLE);
    */

    dPhiGraph.BindWindow();
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(zpr.getModelview());
    RtoR::FunctionRenderer::renderFunction(dPhi, xMin, xMin, xMax - xMin, 100, 100);
}

void R2toR::OutputOpenGL::reshape(int width, int height) {
    //ModelBase::OutputOpenGL::reshape(width, height);

    windowWidth = width;
    windowHeight = height;

    const Real minSize = std::min(Real(windowWidth-statsWindowWidth), Real(windowHeight));
    phiGraph =     Graph(statsWindowWidth, height/2, minSize, minSize/2, 1.1*xMin, 1.1*xMax, 1.1*yMin, 1.1*yMax);
    dPhiGraph =    Graph(statsWindowWidth, 0,        minSize, minSize/2, 1.1*xMin, 1.1*xMax, 1.1*yMin, 1.1*yMax);
    zpr.zprReshape(phiGraph.winX, phiGraph.winY, phiGraph.winW, phiGraph.winH);

}

void R2toR::OutputOpenGL::notifyGLUTKeyboardSpecial(int key, int x, int y) {
}

void R2toR::OutputOpenGL::notifyGLUTKeyboard(unsigned char key, int x, int y) {
    if(key == '2'){
        showAnalytic = !showAnalytic;
        return;
    }
    else if(key == ']'){
        yMin *= 1.1;
        yMax *= 1.1;
    }
    else if(key == '['){
        yMin /= 1.1;
        yMax /= 1.1;
    }
    else if(key == '}'){
        yMin *= 1.5;
        yMax *= 1.5;
    }
    else if(key == '{'){
        yMin /= 1.5;
        yMax /= 1.5;
    }
}

IntPair R2toR::OutputOpenGL::getWindowSizeHint() {
    return {1200, 800};
}

void R2toR::OutputOpenGL::notifyGLUTMouseButton(int button, int dir, int x, int y) {
    zpr.zprMouseButton(button, dir, x, y);
}

void R2toR::OutputOpenGL::notifyGLUTMouseMotion(int x, int y) {
    zpr.zprMouseMotion(x, y);
}

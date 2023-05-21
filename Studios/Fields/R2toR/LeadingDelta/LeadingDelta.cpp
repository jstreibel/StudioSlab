//
// Created by joao on 4/11/23.
//


#include "LeadingDelta.h"
#include "Fields/Mappings/R2toR/Model/FunctionsCollection/AnalyticShockwave2DRadialSymmetry.h"

#include "3rdParty/imgui/imgui.h"

#if USE_VTK
#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkRenderingOpenGL2)
VTK_MODULE_INIT(vtkInteractionStyle)

#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkSphereSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkOpenGLRenderWindow.h>
#endif

namespace R2toR {
    namespace LeadingDelta {
        SpecialRingDelta *ringDelta1 = nullptr;
    }
}

R2toR::LeadingDelta::OutGL::OutGL(Real xMin, Real xMax, Real yMin, Real yMax, Real phiMin, Real phiMax)
        : R2toR::OutputOpenGL(xMin, xMax, yMin, yMax, phiMin, phiMax) {

#if USE_VTK
    // Create VTK objects
    vtkSmartPointer<vtkSphereSource> sphereSource = vtkSmartPointer<vtkSphereSource>::New();
    sphereSource->SetRadius(1.0);
    sphereSource->Update();

    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputConnection(sphereSource->GetOutputPort());

    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);

    vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
    renderer->AddActor(actor);

    vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();

    // Set the existing GLUT window's ID and context to VTK
    renderWindow->SetWindowInfo(
            glutGetWindow());

    renderWindow->AddRenderer(renderer);

    // Set up interactor style
    vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
            vtkSmartPointer<vtkRenderWindowInteractor>::New();
    vtkSmartPointer<vtkInteractorStyleTrackballCamera> interactorStyle =
            vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
    renderWindowInteractor->SetRenderWindow(renderWindow);
    renderWindowInteractor->SetInteractorStyle(interactorStyle);

    renderWindowInteractor->Initialize();
#endif

}

void R2toR::LeadingDelta::OutGL::draw() {
    if (!lastData.hasValidData()) return;

    std::stringstream ss;
    auto &p = Numerics::Allocator::getInstance().getNumericParams();
    const auto t = lastData.getSimTime();
    const auto L = p.getL();
    const auto N = p.getN();

    auto dt = Numerics::Allocator::getInstance().getNumericParams().getdt();
    stats.addVolatileStat(String("t = ") + ToString(t, 4));
    stats.addVolatileStat(String("L = ") + ToString(L));
    stats.addVolatileStat(String("N = ") + ToString(N));
    stats.addVolatileStat(String("N² = ") + ToString(N*N));
    const R2toR::FieldState &fState = *lastData.getFieldData<R2toR::FieldState>();
    auto &phi = fState.getPhi();

    mSectionGraph.clearFunctions();
    mSectionGraph.addFunction(&phi, ColorScheme::graphs[0], "Numeric");


    auto &rd = *R2toR::LeadingDelta::ringDelta1;

    ImGui::Begin("Render");
    static auto deltaRing = true;
    static auto analytic = true;
    static auto timeOffset = .0f;
    static auto a = rd.getA();
    static auto eps = rd.getEps();

    ImGui::Checkbox("Show delta ring", &deltaRing);
    ImGui::Checkbox("Show analytic", &analytic);
    ImGui::SliderFloat("Analytic time offset", &timeOffset, -eps, eps);
    ImGui::End();

    stats.addVolatileStat(String("Ring radius: ") + ToString(rd.getRadius()));
    if(deltaRing) {
        mSectionGraph.addFunction(&rd, ColorScheme::graphs[2], "Ring delta");
    }
    if(analytic){
        RtoR::AnalyticShockwave2DRadialSymmetry radialShockwave;
        radialShockwave.sett(t - dt - double(timeOffset));
        FunctionAzimuthalSymmetry shockwave(&radialShockwave, 1, 0, 0, false);
        mSectionGraph.addFunction(&shockwave, ColorScheme::graphs[3], "Analytic");
    }

    panel->draw(true, true);

#if USE_VTK
    renderWindowInteractor->Render();
#endif
}

IntPair R2toR::LeadingDelta::OutGL::getWindowSizeHint() {
    // 1280 540
    return {3200, 1350};
}




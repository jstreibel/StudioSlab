//
// Created by joao on 4/11/23.
//


#include "LeadingDelta.h"
#include "Mappings/R2toR/Model/FunctionsCollection/AnalyticShockwave2DRadialSymmetry.h"
#include "Base/Controller/Interface/InterfaceManager.h"

#include "Phys/Function/FunctionScale.h"
#include "Phys/DifferentialEquations/2nd-Order/Energy.h"

#include "3rdParty/imgui/imgui.h"


namespace R2toR {
    namespace LeadingDelta {
        SpecialRingDelta *ringDelta1 = nullptr;
        Real ring_tf = -1;
        R2toR::FunctionAzimuthalSymmetry nullFunc(new RtoR::NullFunction);
    }
}

R2toR::LeadingDelta::OutGL::OutGL(Real xMin, Real xMax, Real yMin, Real yMax, Real phiMin, Real phiMax)
        : R2toR::OutputOpenGL(xMin, xMax, yMin, yMax, phiMin, phiMax),
        mEnergyGraph("Energy"), mEnergyRatioGraph("Energy ratio") {

    energyRatioData = Spaces::PointSet::New();
    mEnergyRatioGraph.addPointSet(energyRatioData,  Styles::GetColorScheme()->funcPlotStyles[0],
                                  "Numeric/analytic energy");
    panel->addWindowToColumn(&mEnergyRatioGraph, 0);

    numericEnergyData = Spaces::PointSet::New();
    analyticEnergyData = Spaces::PointSet::New();
    mEnergyGraph.addPointSet(numericEnergyData,  Styles::GetColorScheme()->funcPlotStyles[0], "Numeric energy");
    mEnergyGraph.addPointSet(analyticEnergyData, Styles::GetColorScheme()->funcPlotStyles[1], "Analytic energy");
    panel->addWindowToColumn(&mEnergyGraph, 0);

}

void R2toR::LeadingDelta::OutGL::draw() {
    if (!lastData.hasValidData()) return;

    std::stringstream ss;
    auto &p = Numerics::Allocator::getInstance().getNumericParams();
    const auto t = lastData.getSimTime();
    const auto step = lastData.getSteps();
    const auto L = p.getL();
    const auto N = p.getN();
    const auto h = p.geth();

    const auto ldd = InterfaceManager::getInstance().getInterface("ldd");
    const auto epsilon = *(Real*)ldd->getParameter("eps")->getValue();

    auto dt = Numerics::Allocator::getInstance().getNumericParams().getdt();
    stats.addVolatileStat(String("t = ")     + ToString(t,         4));
    stats.addVolatileStat(String("step = ")  + ToString(step));
    stats.addVolatileStat(String(""));
    stats.addVolatileStat(String("L = ")     + ToString(L));
    stats.addVolatileStat(String("N = ")     + ToString(N));
    stats.addVolatileStat(String("h = ")     + ToString(h,         4, true));
    stats.addVolatileStat(String("eps = ")   + ToString(epsilon,   4, true));
    stats.addVolatileStat(String("eps/h = ") + ToString(epsilon/h, 4));
    stats.addVolatileStat(String(""));
    stats.addVolatileStat(String("L² = ")    + ToString(L*L));
    stats.addVolatileStat(String("N² = ")    + ToString(N*N));
    stats.addVolatileStat(String("h² = ")    + ToString(L*L/(N*N), 4, true));
    stats.addVolatileStat(String(""));


    const R2toR::FieldState &fState = *lastData.getFieldData<R2toR::FieldState>();
    static auto lastStep=0;
    ImGui::Begin("Energy compute");
    static auto radiusDelta = -1.f*(float)epsilon;

    ImGui::DragFloat("Radius delta", &radiusDelta, (float)epsilon*.0025f, -1.5f*epsilon, 1.5f*epsilon, "%.2e");
    auto button = ImGui::Button("Reset");
    if(button) radiusDelta = -(float)epsilon;
    if(step != lastStep)
    {

        Phys::Gordon::Energy energy;
        auto E_radius = t + radiusDelta;
        auto E = energy.computeRadial(fState, E_radius); // energy[fState];
        numericEnergyData->addPoint({t, E});
        const auto a0 = 2. / 3;
        auto analyticEnergy = a0 * M_PI * t * t;
        analyticEnergyData->addPoint({t, analyticEnergy});

        if(analyticEnergy != 0) energyRatioData->addPoint({t, E/analyticEnergy});

        {
            const auto R = E_radius;

            mSectionGraph.clearCurves();
            mSectionGraph.addCurve(RtoR2::StraightLine::New({R, -10}, {R, 10}),
                                   Styles::PlotStyle(Styles::Color{1, 0, 0}, Styles::DotDashed, false, Styles::Nil, 4),
                                   "");
            mSectionGraph.addCurve(RtoR2::StraightLine::New({-R, -10}, {-R, 10}),
                                   Styles::PlotStyle(Styles::Color{1, 0, 0}, Styles::DotDashed, false, Styles::Nil, 4),
                                   "");
        }

        lastStep = step;

    }
    ImGui::End();

    auto &phi = fState.getPhi();

    mSectionGraph.clearFunctions();

    auto &rd = *R2toR::LeadingDelta::ringDelta1;

    ImGui::Begin("Render");
    static auto numeric = true;
    static auto deltaRing = true;
    static auto analytic = true;
    static auto timeOffset = .0f;
    static auto a = rd.getA();
    static auto eps = rd.getEps();

    ImGui::Checkbox("Show numeric", &numeric);
    ImGui::Checkbox("Show delta ring", &deltaRing);
    ImGui::Checkbox("Show analytic", &analytic);
    ImGui::SliderFloat("t_offset", &timeOffset, -eps, eps);
    ImGui::End();

    if(numeric)
        mSectionGraph.addFunction(&phi, "Numeric", Styles::GetColorScheme()->funcPlotStyles[0]);

    stats.addVolatileStat(String("Ring radius: ") + ToString(rd.getRadius()));

    auto scale = eps*1.2;
    auto rdScaledDown = Base::Scale(rd, scale);
    if(deltaRing) {
        R2toR::Function *func = nullptr;
        stats.addVolatileStat(String("ring_tf = ") + ToString(ring_tf));

        auto name = String("Ring delta x") + ToString(scale, 2, true);

        if(ring_tf>t || ring_tf < 0)
            mSectionGraph.addFunction(&rdScaledDown, name, Styles::GetColorScheme()->funcPlotStyles[1]);
        else
            mSectionGraph.addFunction(&nullFunc, name, Styles::GetColorScheme()->funcPlotStyles[1]);
    }

    // Essas funcs precisam ficar do lado de fora do 'if', pra não serem deletadas antes da chamada ao
    // panel->draw
    RtoR::AnalyticShockwave2DRadialSymmetry radialShockwave;
    radialShockwave.sett(t - dt - double(timeOffset));
    FunctionAzimuthalSymmetry shockwave(&radialShockwave, 1, 0, 0, false);
    if(analytic){

        mSectionGraph.addFunction(&shockwave, "Analytic", Styles::GetColorScheme()->funcPlotStyles[2]);
    }

    if(0) {
        stats.addVolatileStat("");
        for (auto &interface: InterfaceManager::getInstance().getInterfaces()) {
            stats.addVolatileStat(interface->getGeneralDescription());
            for (auto &param: interface->getParameters()) {
                stats.addVolatileStat(String("    ") + param->getDescription());
            }
        }
    }


    panel->draw();

#if USE_VTK
    renderWindowInteractor->Render();
#endif
}

IntPair R2toR::LeadingDelta::OutGL::getWindowSizeHint() {
    // 1280 540
    return {3200, 1350};
    // return {1920, 800};
}

void R2toR::LeadingDelta::OutGL::_out(const OutputPacket &outInfo) {
    OutputOpenGL::_out(outInfo);
}

void R2toR::LeadingDelta::OutGL::notifyKeyboard(unsigned char key, int x, int y) {
    if (key == 16) { // glutGetModifiers() & GLUT_ACTIVE_CTRL && (key == 'p' || key == 'P') )
        auto buffer = GLUTUtils::getFrameBuffer();

        auto fileName = String("signum Gordon (2+1 leading-delta) ");
        fileName += InterfaceManager::getInstance().renderParametersToString({"N", "L", "eps", "h"}, "  ");
        fileName += ".png";

        std::cout << fileName << std::endl;

        OpenGLUtils::outputToPNG(buffer, fileName);
    } else {
        OutputOpenGL::notifyKeyboard(key, x, y);
    }
}

void R2toR::LeadingDelta::OutGL::notifyMouseMotion(int x, int y) {
    OutputOpenGL::notifyMouseMotion(x, y);

    if(panel->doesHit(x, y)) panel->notifyMouseMotion(x, y);
}

void R2toR::LeadingDelta::OutGL::notifyMousePassiveMotion(int x, int y) {
    EventListener::notifyMousePassiveMotion(x, y);

    if(panel->doesHit(x, y)) panel->notifyMousePassiveMotion(x, y);
}




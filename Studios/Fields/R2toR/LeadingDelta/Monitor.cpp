//
// Created by joao on 09/06/23.
//

#include "Monitor.h"
#include "LeadingDelta.h"

#include "3rdParty/imgui/imgui.h"
#include "Base/Controller/Interface/InterfaceManager.h"

#include "Phys/Numerics/Allocator.h"
#include "Phys/Function/FunctionScale.h"
#include "Phys/DifferentialEquations/2nd-Order/Energy.h"

#include "Mappings/R2toR/Model/FunctionsCollection/AnalyticShockwave2DRadialSymmetry.h"
#include "Mappings/RtoR/Model/FunctionsCollection/NullFunction.h"
#include "Phys/Function/FunctionScale.h"

R2toR::FunctionAzimuthalSymmetry nullFunc(new RtoR::NullFunction);


R2toR::LeadingDelta::OutGL::OutGL(R2toR::Function::Ptr drivingFunction, Real xMin, Real xMax, Real yMin, Real yMax, Real phiMin, Real phiMax)
        : R2toR::OutputOpenGL(xMin, xMax, yMin, yMax, phiMin, phiMax), drivingFunction(drivingFunction),
          mTotalEnergyGraph("Total energy"), mEnergyGraph("Energy"), mEnergyRatioGraph("Energy ratio"), mSpeedsGraph(xMin, xMax, phiMin, phiMax) {
    energyRatioData = Spaces::PointSet::New();
    mEnergyRatioGraph.addPointSet(energyRatioData,  Styles::GetColorScheme()->funcPlotStyles[0],
                                  "Numeric/analytic energy");
    panel->addWindowToColumn(&mEnergyRatioGraph, 0);


    numericEnergyData = Spaces::PointSet::New();
    analyticEnergyData = Spaces::PointSet::New();
    mEnergyGraph.addPointSet(numericEnergyData,  Styles::GetColorScheme()->funcPlotStyles[0], "Numeric energy");
    mEnergyGraph.addPointSet(analyticEnergyData, Styles::GetColorScheme()->funcPlotStyles[1], "Analytic energy");
    panel->addWindowToColumn(&mEnergyGraph, 0);


    totalEnergyData = Spaces::PointSet::New();
    mTotalEnergyGraph.addPointSet(totalEnergyData,  Styles::GetColorScheme()->funcPlotStyles[0],
                                  "Total analytic energy");
    panel->addWindowToColumn(&mTotalEnergyGraph, 0);

    auto line = new RtoR2::StraightLine({0, yMin},{0, yMax}, yMin, yMax);
    mSpeedsGraph.addSection(line, Styles::Color(1,0,0,1));
    panel->addWindow(&mSpeedsGraph);
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

    const auto ldd = InterfaceManager::getInstance().getInterface("Leading Delta");
    const auto epsilon = *(Real*) ldd->getParameter("eps")->getValueVoid();

    auto dt = Numerics::Allocator::getInstance().getNumericParams().getdt();
    stats.addVolatileStat(String("t = ")     + ToString(t,         4));
    stats.addVolatileStat(String("step = ")  + ToString(step));
    stats.addVolatileStat(String("<\\br>"));
    stats.addVolatileStat(String("L = ")     + ToString(L));
    stats.addVolatileStat(String("N = ")     + ToString(N));
    stats.addVolatileStat(String("h = ")     + ToString(h,         4, true));
    stats.addVolatileStat(String("eps = ")   + ToString(epsilon,   4, true));
    stats.addVolatileStat(String("eps/h = ") + ToString(epsilon/h, 4));
    stats.addVolatileStat(String("<\\br>"));
    stats.addVolatileStat(String("L² = ")    + ToString(L*L));
    stats.addVolatileStat(String("N² = ")    + ToString(N*N));
    stats.addVolatileStat(String("h² = ")    + ToString(L*L/(N*N), 4, true));
    stats.addVolatileStat(String("<\\br>"));


    const R2toR::FieldState &fState = *lastData.getFieldData<R2toR::FieldState>();
    static auto lastStep=0;
    static auto energyIntegrationRadius = (float)(-epsilon);
    static auto lastE = .0;
    static auto lastAnalyticE = .0;
    auto totalE = .0;

    if(0) {
        ImGui::Begin("Energy compute");
        ImGui::DragFloat("Energy integration radius: ", &energyIntegrationRadius, (float) epsilon * 1e-3f,
                         -2.0f * epsilon, 2.0f * epsilon, "%.2e");

        if(ImGui::Button("Reset")) energyIntegrationRadius = -(float)epsilon;

        ImGui::End();
    }

    if(step != lastStep)
    {
        Phys::Gordon::Energy energy;
        auto E_radius = t+energyIntegrationRadius;
        auto E = energy.computeRadial(fState, E_radius); // energy[fState];
        numericEnergyData->addPoint({t, E});
        auto analyticEnergy = (2./3.) * M_PI * t * t;
        analyticEnergyData->addPoint({t, (Real)analyticEnergy});

        if(analyticEnergy != 0) energyRatioData->addPoint({t, E/(Real)analyticEnergy});

        {
            const auto R = E_radius;

            mSectionGraph.clearCurves();
            mSectionGraph.addCurve(RtoR2::StraightLine::New({R, -10}, {R, 10}),
                                   Styles::PlotStyle(Styles::Color{1, 0, 0}, Styles::DotDashed, false, Styles::Nil, 2),
                                   "Numeric wave inside limits");
            mSectionGraph.addCurve(RtoR2::StraightLine::New({-R, -10}, {-R, 10}),
                                   Styles::PlotStyle(Styles::Color{1, 0, 0}, Styles::DotDashed, false, Styles::Nil, 2),
                                   "");
        }

        totalE = energy[fState];
        totalEnergyData->addPoint({t, totalE});

        lastE = E;
        lastAnalyticE = analyticEnergy;

        lastStep = step;

    }

    stats.addVolatileStat(String("E_tot = ")    + ToString(totalE, 2, true));
    stats.addVolatileStat(String("E_num = ")    + ToString(lastE, 2, true));
    stats.addVolatileStat(String("E_an = ")    + ToString(lastAnalyticE, 2, true));
    stats.addVolatileStat(String("E_num/E_an = ")    + ToString(lastE/lastAnalyticE , 2, true));

    auto &phi = fState.getPhi();
    auto &dphidt = fState.getDPhiDt();

    mSectionGraph.clearFunctions();
    mSpeedsGraph.clearFunctions();

    auto &rd = *R2toR::LeadingDelta::ringDelta1;

    static auto numeric = true;
    static auto deltaRing = true;
    static auto analytic = true;
    static auto numericSpeed = true;
    static auto analyticSpeed = true;
    static auto timeOffset = .0f;
    static auto a = rd.getA();
    static auto eps = rd.getEps();

    ImGui::Begin("Render");
    ImGui::SeparatorText("phi");
    ImGui::Checkbox("Show numeric", &numeric);
    ImGui::Checkbox("Show delta ring", &deltaRing);
    ImGui::Checkbox("Show analytic", &analytic);
    ImGui::SameLine();
    ImGui::SliderFloat("t_offset", &timeOffset, -eps, eps, "%.3f");
    ImGui::SeparatorText("dphi/dt");
    ImGui::Checkbox("Show numeric speed", &numericSpeed);
    ImGui::Checkbox("Show analytic speed", &analyticSpeed);
    ImGui::End();

    if(numeric)
        mSectionGraph.addFunction(&phi, "Numeric", Styles::GetColorScheme()->funcPlotStyles[0]);

    stats.addVolatileStat(String("Ring radius: ") + ToString(rd.getRadius()));

    auto scale = eps;
    auto rdScaledDown = Base::Scale(rd, scale);
    if(deltaRing) {
        R2toR::Function *func = nullptr;

        auto name = String("Ring delta x") + ToString(scale, 2, true);

        mSectionGraph.addFunction(&rdScaledDown, name, Styles::GetColorScheme()->funcPlotStyles[1]);
    }

    // Essas funcs precisam ficar do lado de fora do 'if', pra não serem deletadas antes da chamada ao
    // panel->draw
    RtoR::AnalyticShockwave2DRadialSymmetry radialShockwave;
    radialShockwave.sett(t - dt - Real(timeOffset));
    FunctionAzimuthalSymmetry shockwave(&radialShockwave, 1, 0, 0, false);
    if(analytic){

        mSectionGraph.addFunction(&shockwave, "Analytic", Styles::GetColorScheme()->funcPlotStyles[2]);
    }

    if(numericSpeed)
        mSpeedsGraph.addFunction(&dphidt, "Numeric speed", Styles::GetColorScheme()->funcPlotStyles[0]);

    RtoR::AnalyticShockwave2DRadialSymmetry ddtRadialShockwave;
    ddtRadialShockwave.sett(t - dt - Real(timeOffset));
    FunctionAzimuthalSymmetry ddtShockwave(&radialShockwave, 1, 0, 0, false);
    if(analyticSpeed) {
        mSpeedsGraph.addFunction(&ddtShockwave, "Analytic speed", Styles::GetColorScheme()->funcPlotStyles[1]);
    }

    panel->draw();
}

bool R2toR::LeadingDelta::OutGL::notifyKeyboard(unsigned char key, int x, int y) {
    if(EventListener::notifyKeyboard(key, x, y)) return true;

    if (key == 16) { // glutGetModifiers() & GLUT_ACTIVE_CTRL && (key == 'p' || key == 'P') )
        auto buffer = GLUTUtils::getFrameBuffer();

        auto fileName = String("signum Gordon (2+1 leading-delta) ");
        fileName += InterfaceManager::getInstance().renderParametersToString({"N", "L", "eps", "h"}, "  ");
        fileName += ".png";

        OpenGLUtils::outputToPNG(buffer, fileName);

        return true;
    }

    return OutputOpenGL::notifyKeyboard(key, x, y);
}

bool R2toR::LeadingDelta::OutGL::notifyMouseMotion(int x, int y) {
    OutputOpenGL::notifyMouseMotion(x, y);

    if(panel->doesHit(x, y)) return panel->notifyMouseMotion(x, y);

    return false;
}

bool R2toR::LeadingDelta::OutGL::notifyMousePassiveMotion(int x, int y) {
    if(EventListener::notifyMousePassiveMotion(x, y)) return true;

    if(panel->doesHit(x, y)) return panel->notifyMousePassiveMotion(x, y);

    return false;
}

bool R2toR::LeadingDelta::OutGL::notifyMouseButton(int button, int dir, int x, int y) {
    panel->notifyMouseButton(button, dir, x, y);

    return EventListener::notifyMouseButton(button, dir, x, y);
}

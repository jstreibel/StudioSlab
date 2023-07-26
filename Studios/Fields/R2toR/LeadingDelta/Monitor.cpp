//
// Created by joao on 09/06/23.
//

#include "Monitor.h"
#include "LeadingDelta.h"

#include "3rdParty/imgui/imgui.h"
#include "Base/Controller/Interface/InterfaceManager.h"

#include "Phys/Numerics/Allocator.h"
#include "Phys/Function/FunctionScale.h"
#include "Mappings/R2toR/Model/Energy.h"

#include "Mappings/R2toR/Model/FunctionsCollection/AnalyticShockwave2DRadialSymmetry.h"
#include "Mappings/RtoR/Model/FunctionsCollection/NullFunction.h"
#include "Phys/Function/FunctionScale.h"

R2toR::FunctionAzimuthalSymmetry nullFunc(new RtoR::NullFunction);


R2toR::LeadingDelta::OutGL::OutGL(const NumericParams &params, R2toR::Function::Ptr drivingFunction,
                                  Real xMin, Real xMax, Real yMin, Real yMax, Real phiMin, Real phiMax)
: R2toR::OutputOpenGL(params, xMin, xMax, yMin, yMax, phiMin, phiMax)
, drivingFunction(drivingFunction)
, mTotalEnergyGraph("Total energy")
, mEnergyGraph("Energy")
, mEnergyRatioGraph("Energy ratio")
, mSpeedsGraph(xMin,
               xMax,
               phiMin*100,
               phiMax*100,
               "",
               false,
               Numerics::Allocator::GetInstance().getNumericParams().getN())
, mEnergyDensityGraph(xMin, xMax)
{
    {
        energyRatioData = Spaces::PointSet::New();
        mEnergyRatioGraph.addPointSet(energyRatioData,
                                      Styles::GetColorScheme()->funcPlotStyles[0],
                                      "Numeric/analytic energy");
        auto line = RtoR2::StraightLine::New({0, 1},
                                             {Numerics::Allocator::GetInstance().getNumericParams().gett(),
                                              1});
        auto style = Styles::PlotStyle(Styles::Color{1, 0, 0}, Styles::DotDashed, false, Styles::Nil, 2);
        mEnergyRatioGraph.addCurve(line, style, "Target=1.0");
        panel.addWindowToColumn(&mEnergyRatioGraph, 0);
    }

    {
        numericEnergyData = Spaces::PointSet::New();
        analyticEnergyData = Spaces::PointSet::New();
        mEnergyGraph.addPointSet(numericEnergyData,
                                 Styles::GetColorScheme()->funcPlotStyles[0],
                                 "Numeric energy");
        mEnergyGraph.addPointSet(analyticEnergyData,
                                 Styles::GetColorScheme()->funcPlotStyles[2],
                                 "Analytic energy");
        panel.addWindowToColumn(&mEnergyGraph, 0);
    }

    {
        totalEnergyData = Spaces::PointSet::New();
        mTotalEnergyGraph.addPointSet(totalEnergyData,
                                      Styles::GetColorScheme()->funcPlotStyles[0],
                                      "Total analytic energy");
        panel.addWindowToColumn(&mTotalEnergyGraph, 0);

        auto line = new RtoR2::StraightLine({0, yMin},
                                            {0, yMax},
                                            yMin,
                                            yMax);
        mSpeedsGraph.addSection(line, Styles::Color(1, 0, 0, 1));
        panel.addWindow(&mSpeedsGraph);
    }


}

void R2toR::LeadingDelta::OutGL::_out(const OutputPacket &outInfo) {
    OutputOpenGL::_out(outInfo);

    Phys::Gordon::Energy energy;
    auto delta = -(getnSteps())*params.getdt();
    auto E_radius = delta + t + energyIntegrationRadiusDelta;
    auto E = energy.computeRadial_method2(eqState, E_radius);
    numericEnergyData->addPoint({t, E});
    auto analyticEnergy = (2./3.) * M_PI * t * t;
    analyticEnergyData->addPoint({t, (Real)analyticEnergy});

    if(analyticEnergy != 0) energyRatioData->addPoint({t, E/(Real)analyticEnergy});

    if(showEnergyIntegrationLimits)
    {
        const auto R = E_radius;

        mSectionGraph.clearCurves();
        mSpeedsGraph.clearCurves();

        auto line1 = RtoR2::StraightLine::New({R, -10}, {R, 10});
        auto line2 = RtoR2::StraightLine::New({-R, -10}, {-R, 10});
        auto style = Styles::PlotStyle(Styles::Color{1, 0, 0}, Styles::DotDashed, false, Styles::Nil, 2);

        mSectionGraph.addCurve(line1, style, "Numeric wave inside limits");
        mSectionGraph.addCurve(line2, style, "");

        mSpeedsGraph.addCurve(line1, style, "Numeric wave inside limits");
        mSpeedsGraph.addCurve(line2, style, "");

    }

    totalE = energy[eqState];
    totalEnergyData->addPoint({t, totalE});

    lastE = E;
    lastAnalyticE = analyticEnergy;

    lastStep = step;
}

void R2toR::LeadingDelta::OutGL::draw() {
    static auto timer = Timer();
    auto elTime = timer.getElTime_msec();
    timer = Timer();

    auto &rd = *R2toR::LeadingDelta::ringDelta1;

    const auto &p = Numerics::Allocator::GetInstance().getNumericParams();
    const auto L = p.getL();
    const auto N = p.getN();
    const auto h = p.geth();

    const auto ldInterface = InterfaceManager::getInstance().getInterface("R2toR-Leading Delta");
    const auto epsilon = *(Real*) ldInterface->getParameter("eps")->getValueVoid();

    auto dt = Numerics::Allocator::GetInstance().getNumericParams().getdt();
    stats.addVolatileStat(Str("t = ") + ToStr(t, 4));
    stats.addVolatileStat(Str("step = ") + ToStr(step));
    stats.addVolatileStat(Str("steps/frame = ") + ToStr(getnSteps()));
    stats.addVolatileStat(Str("steps/sec: ") + ToStr(getnSteps()/(elTime*1e-3)));
    stats.addVolatileStat(Str("FPS: ") + ToStr(1/(elTime*1e-3)));
    stats.addVolatileStat(Str("<\\br>"));
    stats.addVolatileStat(Str("L = ") + ToStr(L));
    stats.addVolatileStat(Str("N = ") + ToStr(N));
    stats.addVolatileStat(Str("h = ") + ToStr(h, 4, true));
    stats.addVolatileStat(Str("eps = ") + ToStr(epsilon, 4, true));
    stats.addVolatileStat(Str("eps/h = ") + ToStr(epsilon / h, 4));
    stats.addVolatileStat(Str("eps/L = ") + ToStr(epsilon / L, 6));
    stats.addVolatileStat(Str("<\\br>"));
    stats.addVolatileStat(Str("L² = ") + ToStr(L * L));
    stats.addVolatileStat(Str("N² = ") + ToStr(N * N));
    stats.addVolatileStat(Str("h² = ") + ToStr(L * L / (N * N), 4, true));
    stats.addVolatileStat(Str("<\\br>"));

    if(1) {
        ImGui::Begin("Energy compute");
        ImGui::Checkbox("Show energy integration limits", &showEnergyIntegrationLimits);
        ImGui::DragFloat("Energy integration radius: ", &energyIntegrationRadiusDelta, (float) epsilon * 1e-3f,
                         -2.0f * epsilon, 2.0f * epsilon, "%.4f");

        if(ImGui::Button("Reset")) energyIntegrationRadiusDelta = -(float)epsilon;

        ImGui::End();
    }

    stats.addVolatileStat(Str("E_tot = ") + ToStr(totalE, 2, true));
    stats.addVolatileStat(Str("E_num = ") + ToStr(lastE, 2, true));
    stats.addVolatileStat(Str("E_an = ") + ToStr(lastAnalyticE, 2, true));
    stats.addVolatileStat(Str("E_num/E_an = ") + ToStr(lastE / lastAnalyticE, 2, true));

    auto &phi = eqState.getPhi();       phi   .getSpace().syncHost();
    auto &dphidt = eqState.getDPhiDt(); dphidt.getSpace().syncHost();

    mSectionGraph.clearFunctions();
    mSpeedsGraph .clearFunctions();

    static auto showNumeric = true;
    static auto showDeltaRing = true;
    static auto showAnalytic = true;
    static auto showNumericSpeed = true;
    static auto showAnalyticSpeed = true;
    static auto timeOffset = .0f;
    static auto a = rd.getA();
    static auto eps = rd.getEps();

    ImGui::Begin("Render");
    ImGui::SeparatorText("phi");
    ImGui::Checkbox("Show numeric", &showNumeric);
    ImGui::Checkbox("Show delta ring", &showDeltaRing);
    ImGui::Checkbox("Show analytic", &showAnalytic);
    ImGui::SameLine();
    ImGui::SliderFloat("t_offset", &timeOffset, -eps, eps, "%.3f");
    ImGui::SeparatorText("dphi/dt");
    ImGui::Checkbox("Show numeric speed", &showNumericSpeed);
    ImGui::Checkbox("Show analytic speed", &showAnalyticSpeed);
    ImGui::End();

    if(showNumeric)
        mSectionGraph.addFunction(&phi, "Numeric", Styles::GetColorScheme()->funcPlotStyles[0]);

    stats.addVolatileStat(Str("Ring radius: ") + ToStr(rd.getRadius()));
    stats.addVolatileStat(Str("Ring height: ") + ToStr(rd.getA()));

    if(showDeltaRing) {
        if(false) {
            auto scale = 1;
            auto rdScaledDown = Base::Scale(rd, scale);
            auto name = Str("Ring delta x") + ToStr(scale, 2, true);
            mSectionGraph.addFunction(&rdScaledDown, name, Styles::GetColorScheme()->funcPlotStyles[1]);
        } else {
            mSectionGraph.addFunction(&rd, "W(0) lift", Styles::GetColorScheme()->funcPlotStyles[1]);
        }
    }

    // Essas funcs precisam ficar do lado de fora do 'if', pra não serem deletadas antes da chamada ao
    // panel->draw
    RtoR::AnalyticShockwave2DRadialSymmetry radialShockwave;
    radialShockwave.sett(t - dt - Real(timeOffset));
    FunctionAzimuthalSymmetry shockwave(&radialShockwave, 1, 0, 0, false);
    if(showAnalytic)     mSectionGraph.addFunction(
            &shockwave,
            "Analytic",
            Styles::GetColorScheme()->funcPlotStyles[2]);

    if(showNumericSpeed) mSpeedsGraph .addFunction(
            &dphidt,
            "Numeric speed",
            Styles::GetColorScheme()->funcPlotStyles[0]);

    RtoR::AnalyticShockwave2DRadialSymmetryTimeDerivativeB ddtRadialShockwave(2*h);
    ddtRadialShockwave.sett(t - dt - Real(timeOffset));
    FunctionAzimuthalSymmetry ddtShockwave(&ddtRadialShockwave, 1, 0, 0, false);
    if(showAnalyticSpeed)
        mSpeedsGraph.addFunction(&ddtShockwave, "Analytic speed", Styles::GetColorScheme()->funcPlotStyles[2]);

    panel.draw();
}

bool R2toR::LeadingDelta::OutGL::notifyKeyboard(unsigned char key, int x, int y) {
    if(EventListener::notifyKeyboard(key, x, y)) return true;

    if (key == 16) { // glutGetModifiers() & GLUT_ACTIVE_CTRL && (key == 'p' || key == 'P') ) ---> CTRL+p
        auto buffer = GLUTUtils::getFrameBuffer();

        auto fileName = Str("signum Gordon (2+1 leading-delta) ");
        fileName += InterfaceManager::getInstance().renderParametersToString({"N", "L", "eps", "h"}, " ");
        fileName += Str(" t=") + ToStr(t) + " (step " + ToStr(step) + ").png";

        OpenGLUtils::outputToPNG(buffer, fileName);

        return true;
    }

    return OutputOpenGL::notifyKeyboard(key, x, y);
}

bool R2toR::LeadingDelta::OutGL::notifyMouseMotion(int x, int y) {
    OutputOpenGL::notifyMouseMotion(x, y);

    if(panel.doesHit(x, y)) return panel.notifyMouseMotion(x, y);

    return false;
}

bool R2toR::LeadingDelta::OutGL::notifyMousePassiveMotion(int x, int y) {
    if(EventListener::notifyMousePassiveMotion(x, y)) return true;

    if(panel.doesHit(x, y)) return panel.notifyMousePassiveMotion(x, y);

    return false;
}

bool R2toR::LeadingDelta::OutGL::notifyMouseButton(int button, int dir, int x, int y) {
    panel.notifyMouseButton(button, dir, x, y);

    return EventListener::notifyMouseButton(button, dir, x, y);
}

bool R2toR::LeadingDelta::OutGL::notifyMouseWheel(int wheel, int direction, int x, int y) {
    panel.notifyMouseWheel(wheel, direction, x, y);

    return EventListener::notifyMouseWheel(wheel, direction, x, y);
}


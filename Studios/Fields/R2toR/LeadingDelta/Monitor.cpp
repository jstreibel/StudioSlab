//
// Created by joao on 09/06/23.
//

#include "Monitor.h"
#include "LeadingDelta.h"

#include "3rdParty/ImGui.h"

#include "Math/Function/FunctionScale.h"
#include "Math/Function/R2toR/Model/Energy.h"

#include "Math/Function/R2toR/Model/FunctionsCollection/AnalyticShockwave2DRadialSymmetry.h"

#define xMin params.getxMin()
#define xMax params.getxMax()

R2toR::FunctionAzimuthalSymmetry nullFunc(new RtoR::NullFunction);


R2toR::LeadingDelta::OutGL::OutGL(const NumericConfig &params,
                                  R2toR::Function::Ptr drivingFunction, Real phiMin, Real phiMax)
: R2toR::OutputOpenGL(params, phiMin, phiMax)
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
               params.getN()*3)
, mEnergyDensityGraph(xMin, xMax)
{
    energyRatioData = Spaces::PointSet::New();
    mEnergyRatioGraph.addPointSet(energyRatioData,
                                  GetCurrent()->funcPlotStyles[0],
                                  "Numeric/analytic energy");
    panel.addWindowToColumn(&mEnergyRatioGraph, 0);


    numericEnergyData = Spaces::PointSet::New();
    analyticEnergyData = Spaces::PointSet::New();
    mEnergyGraph.addPointSet(numericEnergyData,
                             GetCurrent()->funcPlotStyles[0],
                             "Numeric energy");
    mEnergyGraph.addPointSet(analyticEnergyData,
                             GetCurrent()->funcPlotStyles[1],
                             "Analytic energy");
    panel.addWindowToColumn(&mEnergyGraph, 0);


    totalEnergyData = Spaces::PointSet::New();
    mTotalEnergyGraph.addPointSet(totalEnergyData,
                                  GetCurrent()->funcPlotStyles[0],
                                  "Total analytic energy");
    panel.addWindowToColumn(&mTotalEnergyGraph, 0);

    auto line = new RtoR2::StraightLine({0, xMin},
                                        {0, xMax},
                                        xMin,
                                        xMax);
    mSpeedsGraph.addSection(line, Color(1,0,0,1));
    panel.addWindowToColumn(&mSpeedsGraph, 1);


}

void R2toR::LeadingDelta::OutGL::draw() {
    if(!lastData.hasValidData()) return;

    static auto timer = Timer();
    auto elTime = timer.getElTime_msec();
    timer = Timer();

    auto &rd = *R2toR::LeadingDelta::ringDelta1;

    auto &eqState = *lastData.getEqStateData<R2toR::EquationState>();

    const auto &p = params;
    const auto L = p.getL();
    const auto N = p.getN();
    const auto h = p.geth();

    const auto ldInterface = InterfaceManager::getInstance().getInterface("Leading Delta");
    const auto epsilon = *(Real*) ldInterface->getParameter("eps")->getValueVoid();

    static auto lastStep=0;
    static auto energyIntegrationRadius = (float)(-epsilon);
    static auto lastE = .0;
    static auto lastAnalyticE = .0;

    auto dt = params.getdt();

    stats.addVolatileStat(Str("eps = ") + ToStr(epsilon, 4, true));
    stats.addVolatileStat(Str("eps/h = ") + ToStr(epsilon / h, 4));
    stats.addVolatileStat(Str("eps/L = ") + ToStr(epsilon / L, 6));
    stats.addVolatileStat(Str("<\\br>"));
    stats.addVolatileStat(Str("L² = ") + ToStr(L * L));
    stats.addVolatileStat(Str("N² = ") + ToStr(N * N));
    stats.addVolatileStat(Str("h² = ") + ToStr(L * L / (N * N), 4, true));
    stats.addVolatileStat(Str("<\\br>"));

    auto totalE = .0;

    if(1) {
        ImGui::Begin("Energy compute");
        ImGui::DragFloat("Energy integration radius: ", &energyIntegrationRadius, (float) epsilon * 1e-3f,
                         -2.0f * epsilon, 2.0f * epsilon, "%.2e");

        if(ImGui::Button("Reset")) energyIntegrationRadius = -(float)epsilon;

        ImGui::End();
    }

    if(step != lastStep)
    {
        Fields::KleinGordon::Energy energy;
        auto E_radius = t+energyIntegrationRadius;
        auto E = energy.computeRadial_method2(eqState, E_radius);
        numericEnergyData->addPoint({t, E});
        auto analyticEnergy = (2./3.) * M_PI * t * t;
        analyticEnergyData->addPoint({t, (Real)analyticEnergy});

        if(analyticEnergy != 0) energyRatioData->addPoint({t, E/(Real)analyticEnergy});

        {
            const auto R = E_radius;

            mSectionGraph.clearCurves();
            mSectionGraph.addCurve(RtoR2::StraightLine::New({R, -10}, {R, 10}),
                                   PlotStyle(Color{1, 0, 0}, DotDashedLine, false, Nil, 2),
                                   "Numeric wave inside limits");
            mSectionGraph.addCurve(RtoR2::StraightLine::New({-R, -10}, {-R, 10}),
                                   PlotStyle(Color{1, 0, 0}, DotDashedLine, false, Nil, 2),
                                   "");
        }

        totalE = energy[eqState];
        totalEnergyData->addPoint({t, totalE});

        lastE = E;
        lastAnalyticE = analyticEnergy;

        lastStep = step;

    }

    stats.addVolatileStat(Str("E_tot = ") + ToStr(totalE, 2, true));
    stats.addVolatileStat(Str("E_num = ") + ToStr(lastE, 2, true));
    stats.addVolatileStat(Str("E_an = ") + ToStr(lastAnalyticE, 2, true));
    stats.addVolatileStat(Str("E_num/E_an = ") + ToStr(lastE / lastAnalyticE, 2, true));




    auto &phi = eqState.getPhi();       phi   .getSpace().syncHost();
    auto &dphidt = eqState.getDPhiDt(); dphidt.getSpace().syncHost();


    if(mFieldDisplay.getFunction() == nullptr || true) {
        mFieldDisplay.setFunction(DummyPtr(phi));
    }

    mSectionGraph.clearFunctions();
    mSpeedsGraph .clearFunctions();

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

    if(numeric) mSectionGraph.addFunction(&phi, "Numeric", GetCurrent()->funcPlotStyles[0]);

    stats.addVolatileStat(Str("Ring radius: ") + ToStr(rd.getRadius()));

    auto scale = eps;
    static auto rdScaledDown = Core::Scale(rd, scale);
    if(deltaRing) {
        auto name = Str("Ring delta x") + ToStr(scale, 2, true);

        mSectionGraph.addFunction(&rdScaledDown, name, GetCurrent()->funcPlotStyles[1]);
    }

    // Essas funcs precisam ficar do lado de fora do 'if', pra não serem deletadas antes da chamada ao
    // panel->draw
    static RtoR::AnalyticShockwave2DRadialSymmetry radialShockwave;
    radialShockwave.sett(t - dt - Real(timeOffset));
    static FunctionAzimuthalSymmetry shockwave(&radialShockwave, 1, 0, 0, false);
    if(analytic)     mSectionGraph.addFunction(
            &shockwave,
            "Analytic",
            GetCurrent()->funcPlotStyles[2]);

    if(numericSpeed) mSpeedsGraph .addFunction(
            &dphidt,
            "Numeric speed",
            GetCurrent()->funcPlotStyles[0]);

    static RtoR::AnalyticShockwave2DRadialSymmetryTimeDerivativeB ddtRadialShockwave(2*h);
    ddtRadialShockwave.sett(t - dt - Real(timeOffset));
    static FunctionAzimuthalSymmetry ddtShockwave(&ddtRadialShockwave, 1, 0, 0, false);
    if(analyticSpeed)
        mSpeedsGraph.addFunction(&ddtShockwave, "Analytic speed", GetCurrent()->funcPlotStyles[2]);
}

bool R2toR::LeadingDelta::OutGL::notifyKeyboard(unsigned char key, int x, int y) {
    if(GUIEventListener::notifyKeyboard(key, x, y)) return true;

    if (key == 16) { // glutGetModifiers() & GLUT_ACTIVE_CTRL && (key == 'p' || key == 'P') )
        auto buffer = GLUTUtils::getFrameBuffer();

        auto fileName = Str("signum Gordon (2+1 leading-delta) ");
        fileName += InterfaceManager::getInstance().renderParametersToString({"N", "L", "eps", "h"}, " ");
        fileName += Str(" t=") + ToStr(t) + " (step " + ToStr(step) + ").png";

        OpenGLUtils::outputToPNG(buffer, fileName);

        return true;
    }

    return OpenGLMonitor::notifyKeyboard(key, x, y);
}

bool R2toR::LeadingDelta::OutGL::notifyMouseMotion(int x, int y) {
    OpenGLMonitor::notifyMouseMotion(x, y);

    if(panel.doesHit(x, y)) return panel.notifyMouseMotion(x, y);

    return false;
}

bool R2toR::LeadingDelta::OutGL::notifyMousePassiveMotion(int x, int y) {
    if(GUIEventListener::notifyMousePassiveMotion(x, y)) return true;

    if(panel.doesHit(x, y)) return panel.notifyMousePassiveMotion(x, y);

    return false;
}

bool R2toR::LeadingDelta::OutGL::notifyMouseButton(int button, int dir, int x, int y) {
    panel.notifyMouseButton(button, dir, x, y);

    return GUIEventListener::notifyMouseButton(button, dir, x, y);
}

bool R2toR::LeadingDelta::OutGL::notifyMouseWheel(int wheel, int direction, int x, int y) {
    panel.notifyMouseWheel(wheel, direction, x, y);

    return GUIEventListener::notifyMouseWheel(wheel, direction, x, y);
}


//
// Created by joao on 09/06/23.
//

#include "Monitor.h"

#include <utility>
#include "LeadingDelta.h"

#include "3rdParty/ImGui.h"

#include "Math/Function/FunctionScale.h"
#include "Math/Function/R2toR/Model/Energy.h"
#include "Math/Function/R2toR/Model/FunctionsCollection/AnalyticShockwave2DRadialSymmetry.h"

#include "Graphics/Plot2D/Plotter.h"
#include "Graphics/Plot2D/PlotThemeManager.h"
#include "Math/Function/RtoR2/StraightLine.h"

#define xMin params.getxMin()
#define xMax params.getxMax()

Slab::Math::R2toR::FunctionAzimuthalSymmetry nullFunc(new Slab::Math::RtoR::NullFunction);

namespace Studios::Fields::R2toRLeadingDelta {

    OutGL::OutGL(CountType max_steps, TPointer<Slab::Math::R2toR::Function> drivingFunction)
    : Models::KGR2toR::OutputOpenGL(max_steps), drivingFunction(std::move(drivingFunction))
    , mTotalEnergyGraph ("Total energy")
    , mEnergyGraph      ("Energy")
    , mEnergyRatioGraph ("Energy ratio")
    , mSpeedsGraph      ("Time-derivatives")
    , mEnergyDensityGraph("Energy-densities")
    {
        energyRatioData = New<PointSet>();

        auto theme = Graphics::PlotThemeManager::GetCurrent();
        using Plotter = Graphics::Plotter;

        Plotter::AddPointSet(Dummy(mEnergyRatioGraph), energyRatioData,
                                       theme->FuncPlotStyles[0], "Numeric/analytic energy");
        addWindowToColumn(Dummy(mEnergyRatioGraph), 0);

        numericEnergyData  = New<PointSet>();
        analyticEnergyData = New<PointSet>();

        Plotter::AddPointSet(Dummy(mEnergyGraph), numericEnergyData, theme->FuncPlotStyles[0], "Numeric energy");
        Plotter::AddPointSet(Dummy(mEnergyGraph), analyticEnergyData,theme->FuncPlotStyles[1], "Analytic energy");
        addWindowToColumn(Dummy(mEnergyGraph), 0);


        totalEnergyData = New<PointSet>();
        Plotter::AddPointSet(Dummy(mTotalEnergyGraph), totalEnergyData, theme->FuncPlotStyles[0], "Total analytic energy");
        addWindowToColumn(Dummy(mTotalEnergyGraph), 0);

        // auto line = new RtoR2::StraightLine({0, xMin},
        //                                     {0, xMax},
        //                                     xMin,
        //                                     xMax);
//
        // mSpeedsGraph.addSection(line, Color(1, 0, 0, 1));

        addWindowToColumn(Dummy(mSpeedsGraph), 1);

    }

    void OutGL::ImmediateDraw(const Graphics::FPlatformWindow& PlatformWindow) {
        if (!LastPacket.hasValidData()) return;

        static auto timer = Timer();
        // auto elTime = timer.getElTime_msec();
        timer = Timer();

        auto &rd = *ringDelta1;

        auto &eqState = *LastPacket.GetNakedStateData<R2toR::EquationState>();

        // const auto &p = params;
        // const auto L = p.getL();
        // const auto N = p.getN();
        // const auto h = p.geth();

        const auto ldInterface = FCommandLineInterfaceManager::getInstance().getInterface("Leading Delta");
        const auto epsilon = *(Real *) ldInterface->GetParameter("eps")->GetValueVoid();

        static auto lastStep = 0;
        static auto energyIntegrationRadius = (float) (-epsilon);
        static auto lastE = .0;
        static auto lastAnalyticE = .0;

        // auto dt = params.getdt();

        #define AddStat(stat) getGUIWindow().AddVolatileStat(stat)

        AddStat(Str("eps = ") + ToStr(epsilon, 4, true));
        // AddStat(Str("eps/h = ") + ToStr(epsilon / h, 4));
        // AddStat(Str("eps/L = ") + ToStr(epsilon / L, 6));
        // AddStat(Str("<\\br>"));
        // AddStat(Str("L² = ") + ToStr(L * L));
        // AddStat(Str("N² = ") + ToStr(N * N));
        // AddStat(Str("h² = ") + ToStr(L * L / (N * N), 4, true));
        AddStat(Str("<\\br>"));

        auto totalE = .0;

        if (true) {
            ImGui::Begin("Energy compute");
            ImGui::DragFloat("Energy integration radius: ", &energyIntegrationRadius, (float)epsilon * 1e-3f,
                             -2.f * (float)epsilon, 2.f * (float)epsilon, "%.2e");

            if (ImGui::Button("Reset")) energyIntegrationRadius = -(float) epsilon;

            ImGui::End();
        }

        if (step != lastStep) {
            // fix t = lastPacket.
            // Fields::KleinGordon::Energy energy;
            // auto E_radius = t + energyIntegrationRadius;
            // auto E = energy.computeRadial_method2(eqState, E_radius);
            // numericEnergyData->addPoint({t, E});
            // auto analyticEnergy = (2. / 3.) * M_PI * t * t;
            // analyticEnergyData->addPoint({t, (Real) analyticEnergy});
//
            // if (analyticEnergy != 0) energyRatioData->addPoint({t, E / (Real) analyticEnergy});
//
            // {
            //     const auto R = E_radius;
//
            //     mSectionGraph.clearCurves();
            //     mSectionGraph.addCurve(RtoR2::StraightLine::New({R, -10}, {R, 10}),
            //                            PlotStyle(Color{1, 0, 0}, DotDashedLine, false, Nil, 2),
            //                            "Numeric wave inside limits");
            //     mSectionGraph.addCurve(RtoR2::StraightLine::New({-R, -10}, {-R, 10}),
            //                            PlotStyle(Color{1, 0, 0}, DotDashedLine, false, Nil, 2),
            //                            "");
            // }
//
            // totalE = energy[eqState];
            // totalEnergyData->addPoint({t, totalE});
//
            // lastE = E;
            // lastAnalyticE = analyticEnergy;
//
            // lastStep = step;

        }

        AddStat(Str("E_tot = ") + ToStr(totalE, 2, true));
        AddStat(Str("E_num = ") + ToStr(lastE, 2, true));
        AddStat(Str("E_an = ") + ToStr(lastAnalyticE, 2, true));
        AddStat(Str("E_num/E_an = ") + ToStr(lastE / lastAnalyticE, 2, true));


        auto &phi = eqState.getPhi();
        phi.getSpace().syncHost();
        auto &dphidt = eqState.getDPhiDt();
        dphidt.getSpace().syncHost();


        // if (mFieldDisplay.getFunction() == nullptr || true) {
        //     mFieldDisplay.setFunction(DummyPtr(phi));
        // }

        // mSectionGraph.clearFunctions();
        // mSpeedsGraph.clearFunctions();

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
        ImGui::SliderFloat("t_offset", &timeOffset, -(float)eps, (float)eps, "%.3f");
        ImGui::SeparatorText("dphi/dt");
        ImGui::Checkbox("Show numeric speed", &numericSpeed);
        ImGui::Checkbox("Show analytic speed", &analyticSpeed);
        ImGui::End();

        // if (numeric) mSectionGraph.addFunction(&phi, "Numeric", GetCurrent()->funcPlotStyles[0]);

        AddStat(Str("Ring radius: ") + ToStr(rd.getRadius()));

        // auto scale = eps;
        // static auto rdScaledDown = Core::Scale(rd, scale);
        // if (deltaRing) {
        //     auto name = Str("Ring delta x") + ToStr(scale, 2, true);
//
        //     mSectionGraph.addFunction(&rdScaledDown, name, GetCurrent()->funcPlotStyles[1]);
        // }


        // // Essas funcs precisam ficar do lado de fora do 'if', pra não serem deletadas antes da chamada ao
        // // panel->draw
        // static RtoR::AnalyticShockwave2DRadialSymmetry radialShockwave;
        // radialShockwave.sett(t - dt - Real(timeOffset));
        // static FunctionAzimuthalSymmetry shockwave(&radialShockwave, 1, 0, 0, false);
        // if (analytic)
        //     mSectionGraph.addFunction(
        //             &shockwave,
        //             "Analytic",
        //             GetCurrent()->funcPlotStyles[2]);
//
        // if (numericSpeed)
        //     mSpeedsGraph.addFunction(
        //             &dphidt,
        //             "Numeric speed",
        //             GetCurrent()->funcPlotStyles[0]);
//
        // static RtoR::AnalyticShockwave2DRadialSymmetryTimeDerivativeB ddtRadialShockwave(2 * h);
        // ddtRadialShockwave.sett(t - dt - Real(timeOffset));
        // static FunctionAzimuthalSymmetry ddtShockwave(&ddtRadialShockwave, 1, 0, 0, false);
        // if (analyticSpeed)
        //     mSpeedsGraph.addFunction(&ddtShockwave, "Analytic speed", GetCurrent()->funcPlotStyles[2]);
    }

}
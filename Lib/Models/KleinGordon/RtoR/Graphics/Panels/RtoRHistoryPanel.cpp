//
// Created by joao on 6/17/24.
//

#include "3rdParty/ImGui.h"
#include "Core/Controller/CommandLine/CLInterfaceManager.h"

#include "RtoRHistoryPanel.h"

#include "Graphics/Plot2D/Artists/R2SectionArtist.h"
#include "Graphics/Plot2D/Artists/ParametricCurve2DArtist.h"
#include "Graphics/Plot2D/PlotThemeManager.h"
#include "Graphics/Plot2D/Plotter.h"

namespace Slab::Models::KGRtoR {

    RtoRHistoryPanel::RtoRHistoryPanel
    (const Pointer<KGNumericConfig> &params,
     GUIWindow &guiWindow,
     KGEnergy &hamiltonian)
    : RtoRPanel(params, guiWindow, hamiltonian, "Histories", "Panel to view 1-d sections of histories.")
    {
        fix xMin = params->getxMin();
        fix xMax = params->getxMax();
        fix h = params->geth();
        fix kMin = 0;
        fix kMax = M_PI/h;
        xLine = New<Math::RtoR2::StraightLine>(Real2D{xMin, 0.0}, Real2D{xMax,0.0}, xMin, xMax);
        kLine = New<Math::RtoR2::StraightLine>(Real2D{kMin, 0.0}, Real2D{kMax,0.0}, kMin, kMax);

        auto style = Graphics::PlotThemeManager::GetCurrent()->funcPlotStyles[0];
        style.thickness = 3;
        auto xArtie = New<Graphics::ParametricCurve2DArtist>(xLine, style);
        auto kArtie = New<Graphics::ParametricCurve2DArtist>(kLine, style);

        xSection = New<Plot2DWindow>("Spacial time slice");
        xSpaceHistory = New<Plot2DWindow>();
        xSpaceHistory->addArtist(xArtie, 100);
        xSection->getRegion().setReference_xMax(xSpaceHistory->getRegion().getReference_xMax());
        xSection->getRegion().setReference_xMin(xSpaceHistory->getRegion().getReference_xMin());

        kSection = New<Plot2DWindow>("k-space time slice");
        kSpaceHistory = New<Plot2DWindow>();
        kSpaceHistory->addArtist(kArtie, 100);
        kSection->getRegion().setReference_xMax(kSpaceHistory->getRegion().getReference_xMax());
        kSection->getRegion().setReference_xMin(kSpaceHistory->getRegion().getReference_xMin());
        kSection->getAxisArtist().setHorizontalAxisLabel("k");
        kSection->getAxisArtist().setVerticalAxisLabel("A");

        xSpaceHistory->getRegion().setReference_yMin(kSpaceHistory->getRegion().getReference_yMin());
        xSpaceHistory->getRegion().setReference_yMax(kSpaceHistory->getRegion().getReference_yMax());

        addWindow(xSpaceHistory);
        addWindow(xSection);
        addWindow(kSpaceHistory, true);
        addWindow(kSection);

        {
            auto kParam = Core::CLInterfaceManager::getInstance().getParameter("harmonic");

            if(kParam != nullptr) {
                using Tick = Graphics::AxisArtist::Tick;
                auto unit = Constants::π;
                Graphics::AxisArtist::Ticks ticks;

                ticks.push_back(Tick{0, "0"});

                fix dk    = M_PI / params->getL();
                fix k_0 = dk * kParam->getValueAs<Real>();
                auto k_max = M_PI / params->geth();

                Real k_val = k_0;
                Real Δk = k_max / 50;
                do {
                    ticks.push_back(Tick{k_val, unit(k_val, 0)});
                    k_val += Δk;
                } while (k_val < k_max);

                auto &axisArtist1 = kSpaceHistory->getAxisArtist();
                axisArtist1.setHorizontalUnit(unit);
                axisArtist1.setHorizontalAxisTicks(ticks);

                auto &axisArtist2 = kSection     ->getAxisArtist();
                axisArtist2.setHorizontalUnit(unit);
                axisArtist2.setHorizontalAxisTicks(ticks);
            }
        }
    }

    void RtoRHistoryPanel::draw() {
        guiWindow.begin();

        static float t=0;
        auto t_max = (float)lastPacket.getSteps()*params->getdt();
        if(ImGui::SliderFloat("t##poopsie", &t, 0, t_max) |
           ImGui::DragFloat("t##choopsie", &t, 5.e-5f*t_max, 0, t_max)){
            fix xMin = params->getxMin();
            fix xMax = params->getxMax();
            fix h = params->geth();
            fix kMin = 0;
            fix kMax = M_PI/h;

            fix x_0 = Real2D{xMin, t};
            fix x_f = Real2D{xMax, t};
            fix k_0 = Real2D{kMin, t};
            fix k_f = Real2D{kMax, t};

            xLine->getx0() = x_0;
            xLine->getr() = x_f-x_0;

            kLine->getx0() = k_0;
            kLine->getr() = k_f-k_0;
        }


        guiWindow.end();

        RtoRPanel::draw();
    }

    void RtoRHistoryPanel::setSimulationHistory(R2toR::NumericFunction_constptr simulationHistory,
                                                const R2toRFunctionArtist_ptr &simHistoryGraph) {
        RtoRPanel::setSimulationHistory(simulationHistory, simHistoryGraph);

        auto sectionArtist = Graphics::Plotter::AddR2Section(xSection, simulationHistory, "ϕ");
        auto style = Graphics::PlotThemeManager::GetCurrent()->funcPlotStyles[1];
        sectionArtist->addSection(xLine, style.clone(), "ϕ(0,x)");

        xSpaceHistory->addArtist(simHistoryGraph);
    }

    void
    RtoRHistoryPanel::setSpaceFourierHistory(R2toR::NumericFunction_constptr sftHistory, const DFTDataHistory &history,
                                             const R2toRFunctionArtist_ptr &sftHistoryGraph) {
        RtoRPanel::setSpaceFourierHistory(sftHistory, history, sftHistoryGraph);

        auto sectionArtist = Graphics::Plotter::AddR2Section(kSection, sftHistory, "ℱₓ(k)");
        auto style = Graphics::PlotThemeManager::GetCurrent()->funcPlotStyles[1];
        style.setPrimitive(VerticalLines);
        style.filled = false;
        sectionArtist->addSection(kLine, style.clone(), "");
        sectionArtist->setSamples(sftHistory->getN());

        kSpaceHistory->addArtist(sftHistoryGraph);
    }


} // Slab::Models::RGRtoR
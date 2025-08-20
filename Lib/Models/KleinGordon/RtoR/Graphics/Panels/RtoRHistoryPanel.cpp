//
// Created by joao on 6/17/24.
//

#include "3rdParty/ImGui.h"
#include "../../../../../Core/Controller/InterfaceManager.h"

#include "RtoRHistoryPanel.h"

#include "Graphics/Plot2D/Artists/R2SectionArtist.h"
#include "Graphics/Plot2D/Artists/ParametricCurve2DArtist.h"
#include "Graphics/Plot2D/PlotThemeManager.h"
#include "Graphics/Plot2D/Plotter.h"

namespace Slab::Models::KGRtoR {

    FRtoRHistoryPanel::FRtoRHistoryPanel
    (const TPointer<FKGNumericConfig> &params,
     FGUIWindow &guiWindow,
     FKGEnergy &hamiltonian)
    : FRtoRPanel(params, guiWindow, hamiltonian, "Histories", "Panel to view 1-d sections of histories.")
    {
        fix xMin = params->Get_xMin();
        fix xMax = params->getxMax();
        fix h = params->geth();
        fix kMin = 0;
        fix kMax = M_PI/h;
        xLine = New<Math::RtoR2::StraightLine>(Real2D{xMin, 0.0}, Real2D{xMax,0.0}, xMin, xMax);
        kLine = New<Math::RtoR2::StraightLine>(Real2D{kMin, 0.0}, Real2D{kMax,0.0}, kMin, kMax);

        auto style = Graphics::PlotThemeManager::GetCurrent()->FuncPlotStyles[0];
        style.thickness = 3;
        auto xArtie = New<Graphics::ParametricCurve2DArtist>(xLine, style);
        auto kArtie = New<Graphics::ParametricCurve2DArtist>(kLine, style);

        xSection = New<FPlot2DWindow>("Spacial time slice");
        xSpaceHistory = New<FPlot2DWindow>("Position space history");
        xSpaceHistory->AddArtist(xArtie, 100);
        xSection->GetRegion().setReference_xMax(xSpaceHistory->GetRegion().getReference_xMax());
        xSection->GetRegion().setReference_xMin(xSpaceHistory->GetRegion().getReference_xMin());

        kSection = New<FPlot2DWindow>("k-space time slice");
        kSpaceHistory = New<FPlot2DWindow>("Momentum space history");
        kSpaceHistory->AddArtist(kArtie, 100);
        kSection->GetRegion().setReference_xMax(kSpaceHistory->GetRegion().getReference_xMax());
        kSection->GetRegion().setReference_xMin(kSpaceHistory->GetRegion().getReference_xMin());
        kSection->GetAxisArtist().SetHorizontalAxisLabel("k");
        kSection->GetAxisArtist().setVerticalAxisLabel("A");

        xSpaceHistory->GetRegion().setReference_yMin(kSpaceHistory->GetRegion().getReference_yMin());
        xSpaceHistory->GetRegion().setReference_yMax(kSpaceHistory->GetRegion().getReference_yMax());

        AddWindow(xSpaceHistory);
        AddWindow(xSection);
        AddWindow(kSpaceHistory, true);
        AddWindow(kSection);

        {
            auto kParam = Core::FInterfaceManager::GetInstance().GetParameter("harmonic");

            if(kParam != nullptr) {
                using Tick = Graphics::FAxisArtist::Tick;
                auto unit = Constants::π;
                Graphics::FAxisArtist::Ticks ticks;

                ticks.push_back(Tick{0, "0"});

                fix dk    = M_PI / params->GetL();
                fix k_0 = dk * kParam->GetValueAs<DevFloat>();
                auto k_max = M_PI / params->geth();

                DevFloat k_val = k_0;
                DevFloat Δk = k_max / 50;
                do {
                    ticks.push_back(Tick{k_val, unit(k_val, 0)});
                    k_val += Δk;
                } while (k_val < k_max);

                auto &axisArtist1 = kSpaceHistory->GetAxisArtist();
                axisArtist1.setHorizontalUnit(unit);
                axisArtist1.setHorizontalAxisTicks(ticks);

                auto &axisArtist2 = kSection     ->GetAxisArtist();
                axisArtist2.setHorizontalUnit(unit);
                axisArtist2.setHorizontalAxisTicks(ticks);
            }
        }
    }

    void FRtoRHistoryPanel::ImmediateDraw(const FPlatformWindow& PlatformWindow) {
        guiWindow.AddExternalDraw([this]() {
            static float t = 0;
            auto t_max = (float) LastPacket.GetSteps() * Params->Getdt();
            if (ImGui::SliderFloat("t##poopsie", &t, 0, t_max) |
                ImGui::DragFloat("t##choopsie", &t, 5.e-5f * t_max, 0, t_max)) {
                fix xMin = Params->Get_xMin();
                fix xMax = Params->getxMax();
                fix h = Params->geth();
                fix kMin = 0;
                fix kMax = M_PI / h;

                fix x_0 = Real2D{xMin, t};
                fix x_f = Real2D{xMax, t};
                fix k_0 = Real2D{kMin, t};
                fix k_f = Real2D{kMax, t};

                xLine->getx0() = x_0;
                xLine->getr() = x_f - x_0;

                kLine->getx0() = k_0;
                kLine->getr() = k_f - k_0;
            }
        });

        FRtoRPanel::ImmediateDraw(PlatformWindow);
    }

    void FRtoRHistoryPanel::SetSimulationHistory(TPointer<const R2toR::FNumericFunction>
 simulationHistory,
                                                const R2toRFunctionArtist_ptr &simHistoryGraph) {
        FRtoRPanel::SetSimulationHistory(simulationHistory, simHistoryGraph);

        auto sectionArtist = Graphics::FPlotter::AddR2Section(xSection, simulationHistory, "ϕ");
        auto style = Graphics::PlotThemeManager::GetCurrent()->FuncPlotStyles[1];
        sectionArtist->addSection(xLine, style.clone(), "ϕ(0,x)");

        xSpaceHistory->AddArtist(simHistoryGraph);
    }

    void
    FRtoRHistoryPanel::SetSpaceFourierHistory(TPointer<const R2toR::FNumericFunction>
 sftHistory, const FDFTDataHistory &history,
                                             const R2toRFunctionArtist_ptr &sftHistoryGraph) {
        FRtoRPanel::SetSpaceFourierHistory(sftHistory, history, sftHistoryGraph);

        auto sectionArtist = Graphics::FPlotter::AddR2Section(kSection, sftHistory, "ℱₓ(k)");
        auto style = Graphics::PlotThemeManager::GetCurrent()->FuncPlotStyles[1];
        style.setPrimitive(VerticalLines);
        style.filled = false;
        sectionArtist->addSection(kLine, style.clone(), "");
        sectionArtist->setSamples(sftHistory->getN());

        kSpaceHistory->AddArtist(sftHistoryGraph);
    }

    auto FRtoRHistoryPanel::get_kSectionWindow() -> TPointer<Graphics::FPlot2DWindow> {
        return kSection;
    }
} // Slab::Models::RGRtoR
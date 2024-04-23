//
// Created by joao on 20/04/24.
//

#include "RtoRFourierPanel.h"

#include "Graphics/Graph/StylesManager.h"
#include "imgui.h"
#include "Maps/R2toR/Model/R2toRDiscreteFunctionCPU.h"
#include "Maps/RtoR/Calc/DFTInverse.h"

namespace Graphics {

    RtoRFourierPanel::RtoRFourierPanel(const NumericConfig &params, RtoR::KGEnergy &hamiltonian, GUIWindow &guiWindow)
    : RtoRPanel(params, guiWindow, hamiltonian, "ℝ↦ℝ Fourier panel", "Fourier analysis panel")
    , inverseFTDisplay(new FlatFieldDisplay("ℱₖ⁻¹[ℱ]"))
    , cutoffLine({kFilterCutoff, -10.0}, {kFilterCutoff, params.gett()+10.0})
    {
        addWindow(inverseFTDisplay);
    }

    void RtoRFourierPanel::draw() {
        guiWindow.begin();
        if(ImGui::CollapsingHeader("Cutoff filter")){
            fix kMax = M_PI/params.geth();
            auto k = (float)kFilterCutoff;

            if(ImGui::SliderFloat("cutoff k", &k, 0.0, (float)kMax)){
                kFilterCutoff = k;
                fix t = params.gett();
                cutoffLine = RtoR2::StraightLine({kFilterCutoff, -10.0}, {kFilterCutoff, t+10.0});

                spaceFTHistoryGraph->clearCurves();
                spaceFTHistoryGraph->addCurve(DummyPtr(cutoffLine), Math::StylesManager::GetCurrent()->funcPlotStyles[0], "k cutoff");
            }

            static int selected = 0;
            ImGui::RadioButton("High-pass", &selected, 0);
            ImGui::RadioButton("Low-pass", &selected, 1);

            if(ImGui::Button("Compute ℱₖ⁻¹")){
                refreshInverseDFT();
            }
        }
        guiWindow.end();

        WindowPanel::draw();
    }

    void RtoRFourierPanel::setSimulationHistory(std::shared_ptr<const R2toR::DiscreteFunction> simulationHistory,
                                                std::shared_ptr<HistoryDisplay> simHistoryGraph) {
        RtoRPanel::setSimulationHistory(simulationHistory, simHistoryGraph);

        addWindow(simHistoryGraph, true);
    }

    void RtoRFourierPanel::setSpaceFourierHistory(std::shared_ptr<const R2toR::DiscreteFunction> sftHistory,
                                                  const SimHistory_DFT::DFTDataHistory &dftData,
                                                  std::shared_ptr<HistoryDisplay> sftHistoryGraph) {
        RtoRPanel::setSpaceFourierHistory(sftHistory, dftData, sftHistoryGraph);

        addWindow(spaceFTHistoryGraph, true, .5);

        spaceFTHistoryGraph->addCurve(DummyPtr(cutoffLine), Math::StylesManager::GetCurrent()->funcPlotStyles[0], "k cutoff");
    }

    void RtoRFourierPanel::refreshInverseDFT() {
        assert((sizeof(Real)==sizeof(double)) && " make sure this code is compatible with fftw3");

        fix xMin = RtoRPanel::params.getxMin();
        fix L = RtoRPanel::params.getL();
        fix N = RtoRPanel::params.getN();
        fix hx = L/N;

        fix tMin = (*dftData)[0].t;
        fix t = dftData->back().t;
        fix n = dftData->size();
        fix ht = (t-tMin)/n;

        std::shared_ptr<R2toR::DiscreteFunction_CPU>
                rebuiltHistory(new R2toR::DiscreteFunction_CPU(N, n, xMin, tMin, hx, ht));

        Log::Info(__PRETTY_FUNCTION__ + Str(" STARTED computing inverse DFT."));
        int _n = 0;
        for(auto &data : *dftData){
            auto func = RtoR::DFTInverse::Compute(data.result, xMin, L,
                                                  RtoR::DFTInverse::OriginalFunctionIsRealValued);

            auto *out = &rebuiltHistory->At(0, _n);
            auto *in = &func->getSpace().getHostData(true)[0];

            memcpy(out, in, N*sizeof(Real));

            ++_n;
        }

        Log::Info(__PRETTY_FUNCTION__ + Str(" FINISHED computing inverse DFT."));

        inverseFTDisplay->removeFunction(inverseDFT);
        inverseDFT = rebuiltHistory;
        inverseFTDisplay->addFunction(inverseDFT, "ℱ⁻¹");
    }

} // Graphics

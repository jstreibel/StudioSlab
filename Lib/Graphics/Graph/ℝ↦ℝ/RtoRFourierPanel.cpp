//
// Created by joao on 20/04/24.
//

#include "RtoRFourierPanel.h"

#include "Graphics/Graph/StylesManager.h"
#include "imgui.h"
#include "Math/Function/R2toR/Model/R2toRDiscreteFunctionCPU.h"
#include "Math/Function/RtoR/Calc/DFTInverse.h"
#include "Graphics/Window/WindowContainer/WindowRow.h"

namespace Graphics {

    RtoRFourierPanel::RtoRFourierPanel(const NumericConfig &params, RtoR::KGEnergy &hamiltonian, GUIWindow &guiWindow)
    : RtoRPanel(params, guiWindow, hamiltonian, "ℝ↦ℝ Fourier panel", "Fourier analysis panel")
    , inverseFTDisplay(new FlatFieldDisplay("ℱₖ⁻¹[ℱ]"))
    , timeFTDisplay(new FlatFieldDisplay("ℱₜ[ϕ](ω,x)"))
    , cutoffLine({kFilterCutoff, -10.0}, {kFilterCutoff, params.gett()+10.0})
    {
        inverseFTDisplay->setColorMap(ColorMaps["BrBG"].inverse());
    }

    void RtoRFourierPanel::draw() {
        static bool firstRun = true;
        if(firstRun) {
            firstRun = false;
            // auto windowRow = new WindowRow();
            addWindow(timeFTDisplay);
            addWindow(simulationHistoryGraph);
            addWindow(inverseFTDisplay, true);
            addWindow(spaceFTHistoryGraph);

            arrangeWindows();
        }

        guiWindow.begin();

        if(ImGui::CollapsingHeader("Cutoff filter")){
            //this->dftData
            fix kMax = M_PI/params.geth();
            auto k = (float)kFilterCutoff;

            static int selected = 0;
            ImGui::RadioButton("High-pass", &selected, 0);
            ImGui::RadioButton("Low-pass", &selected, 1);

            if(ImGui::SliderFloat("cutoff k", &k, 0.0, (float)kMax)){
                kFilterCutoff = k;
                fix t = params.gett();
                cutoffLine = RtoR2::StraightLine({kFilterCutoff, -10.0}, {kFilterCutoff, t+10.0});

                spaceFTHistoryGraph->clearCurves();
                spaceFTHistoryGraph->addCurve(DummyPtr(cutoffLine), StylesManager::GetCurrent()->funcPlotStyles[0], "k cutoff");

                if(selected==0) {
                    RtoR::DFTInverse::HighPass lowPass(kFilterCutoff);
                    refreshInverseDFT(&lowPass);
                    // Log::Info(Str("Filtered kMax = ") + ToStr(lowPass.kMax));
                } else if(selected==1) {
                    RtoR::DFTInverse::LowPass highPass(kFilterCutoff);
                    refreshInverseDFT(&highPass);
                    // Log::Info(Str("Filtered kMax = ") + ToStr(highPass.kMax));
                }
            }

            // if(ImGui::Button("Compute ℱₖ⁻¹"))
            // {
            //     if(selected==0) {
            //         RtoR::DFTInverse::LowPass lowPass(kFilterCutoff);
            //         refreshInverseDFT(&lowPass);
            //     } else if(selected==1) {
            //         RtoR::DFTInverse::HighPass highPass(kFilterCutoff);
            //         refreshInverseDFT(&highPass);
            //     }
            // }
        }

        if(ImGui::CollapsingHeader("Time FT")){
            static auto t₀=.0f;
            static fix tMax =(float)RtoRPanel::params.gett();
            static auto t_f = tMax;

            if(ImGui::Button("Compute"))
                computeTimeDFT(t₀, t_f);

            ImGui::SliderFloat("tₘᵢₙ", &t₀, .0f, t_f);
            ImGui::SliderFloat("tₘₐₓ", &t_f, t₀, tMax);
        }

        guiWindow.end();

        WindowPanel::draw();
    }

    void RtoRFourierPanel::setSpaceFourierHistory(std::shared_ptr<const R2toR::DiscreteFunction> sftHistory,
                                                  const DFTDataHistory &dftData,
                                                  std::shared_ptr<HistoryDisplay> sftHistoryGraph) {
        RtoRPanel::setSpaceFourierHistory(sftHistory, dftData, sftHistoryGraph);

        spaceFTHistoryGraph->addCurve(DummyPtr(cutoffLine), StylesManager::GetCurrent()->funcPlotStyles[0], "k cutoff");
    }

    void RtoRFourierPanel::refreshInverseDFT(RtoR::DFTInverse::Filter *filter) {
        assert((sizeof(Real)==sizeof(double)) && " make sure this code is compatible with fftw3");

        fix xMin = RtoRPanel::params.getxMin();
        fix L = RtoRPanel::params.getL();
        fix N = (*dftData)[0].result.modeCount();
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
                                                  filter);

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

    void RtoRFourierPanel::computeTimeDFT(Real t₀, Real t_f) {
        fix N = simulationHistory->getN();
        fix xMin = simulationHistory->getDomain().xMin;
        fix dx = simulationHistory->getDomain().getLx()/N;

        fix Mₜ = simulationHistory->getM();
        fix dt = simulationHistory->getDomain().getLy()/Mₜ;
        fix Δt = t_f-t₀;
        fix __M = (Count)floor(Δt/dt);
        fix M = __M%2==0 ? __M : __M-1;
        fix m = M/2 + 1;

        fix dk = 2*M_PI/Δt;

        timeFTDisplay->removeFunction(timeDFT);

        timeDFT = std::shared_ptr<R2toR::DiscreteFunction>(new R2toR::DiscreteFunction_CPU(N, m, xMin, 0, dx, dk));
        RtoR::DiscreteFunction_CPU tempSpace(M, .0, dk*M);

        fix j₀ = floor(t₀/dt);
        for(auto i=0; i<N; ++i){
            auto &spaceData_temp = tempSpace.getSpace().getHostData(false);

            for(auto j=0; j<M; ++j) spaceData_temp[j] = simulationHistory->At(i, j₀+j);

            auto dftMagnitudes = RtoR::DFT::Compute(tempSpace).getMagnitudes();

            assert(dftMagnitudes->count()==m);

            for(auto k=0; k<m; ++k) {
                fix &pt = dftMagnitudes->getPoints()[k];
                fix A = pt.y;
                fix ω = pt.x;

                timeDFT->At(i, k) = A;
            }
        }

        Str timeInterval = ToStr(t₀) + " ≤ t ≤ " + ToStr(t_f);
        timeFTDisplay->addFunction(timeDFT, Str("ℱₜ[ϕ](ω,x), ") + timeInterval);
    }
} // Graphics

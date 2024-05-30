//
// Created by joao on 20/04/24.
//

#include "RtoRFourierPanel.h"

#include "Graphics/Graph/PlotThemeManager.h"
#include "imgui.h"
#include "Graphics/Graph/Plotter.h"

namespace Slab::Models::KGRtoR {

    RtoRFourierPanel::RtoRFourierPanel(const NumericConfig &params, KGEnergy &hamiltonian, GUIWindow &guiWindow)
    : RtoRPanel(params, guiWindow, hamiltonian, "ℝ↦ℝ Fourier panel", "Fourier analysis panel")
    , cutoffLine({kFilterCutoff, -10.0}, {kFilterCutoff, params.gett()+10.0})
    {
        inverseDFTArtist->setLabel("ℱₖ⁻¹(t, x)");
        // inverseDFTDisplay->addArtist(inverseDFTArtist);
        // inverseDFTDisplay->getAxisArtist().setHorizontalAxisLabel("x");
        // inverseDFTDisplay->getAxisArtist().setVerticalAxisLabel("t");

        timeDFTArtist->setLabel("ℱₜ(ω, x)");
        timeDFTDisplay->addArtist(timeDFTArtist);
        timeDFTDisplay->getAxisArtist().setHorizontalAxisLabel("x");
        timeDFTDisplay->getAxisArtist().setVerticalAxisLabel("ω");

    }

    void RtoRFourierPanel::draw() {
        static bool firstRun = true;
        if(firstRun) {
            firstRun = false;
            // auto windowRow = new WindowRow();
            addWindow(timeDFTDisplay);
            addWindow(simulationHistoryGraph);
            addWindow(inverseDFTDisplay, true);
            addWindow(spaceFTHistoryGraph);

            arrangeWindows();
        }

        guiWindow.begin();

        if(ImGui::CollapsingHeader("Cutoff filter")){
            //this->dftData
            fix kMax = M_PI/params.geth();
            auto k = (float)kFilterCutoff;

            static bool needRefresh = false;
            static int selected = 0;
            static bool autoRefresh = false;
            if(ImGui::Checkbox("Auto##space_dft", &autoRefresh) && autoRefresh) needRefresh = true;

            if(ImGui::RadioButton("High-pass", &selected, 0) | ImGui::RadioButton("Low-pass", &selected, 1))
                needRefresh = true;

            if(ImGui::SliderFloat("cutoff k", &k, 0.0, (float)kMax)){
                kFilterCutoff = k;
                fix t = params.gett();
                cutoffLine.getx0() = {kFilterCutoff, -10.0};
                cutoffLine.getr() = {0, 10.0+t};

                needRefresh = true;
            }


            if((autoRefresh || ImGui::Button("Compute ℱₖ⁻¹")) && needRefresh) {
                if (selected == 1) {
                    RtoR::DFTInverse::LowPass lowPass(kFilterCutoff);
                    refreshInverseDFT(&lowPass);
                } else if (selected == 0) {
                    RtoR::DFTInverse::HighPass highPass(kFilterCutoff);
                    refreshInverseDFT(&highPass);
                }

                needRefresh = false;
            }
        }

        if(ImGui::CollapsingHeader("Time FT")){
            static auto t₀=.0f;
            static fix tMax =(float)RtoRPanel::params.gett();
            static auto t_f = tMax;
            static auto autoUpdate = false;

            ImGui::Checkbox("Auto##time_dft", &autoUpdate);

            ImGui::BeginDisabled(autoUpdate);
            if(ImGui::Button("Compute"))
                computeTimeDFT(t₀, t_f);
            ImGui::EndDisabled();

            if(ImGui::SliderFloat("tₘᵢₙ", &t₀, .0f, t_f) | ImGui::SliderFloat("tₘₐₓ", &t_f, t₀, tMax))
                if(autoUpdate) computeTimeDFT(t₀, t_f);

        }

        guiWindow.end();

        WindowPanel::draw();
    }

    void
    RtoRFourierPanel::setSpaceFourierHistory(R2toR::DiscreteFunction_constptr sftHistory,
                                             const DFTDataHistory &dftDataHistory,
                                             const R2toRFunctionArtist_ptr &dftFunctionArtist) {

        RtoRPanel::setSpaceFourierHistory(sftHistory, dftDataHistory, dftFunctionArtist);

        spaceFTHistoryGraph = Slab::New<PlottingWindow>();
        spaceFTHistoryGraph->addArtist(dftFunctionArtist);

        Graphics::Plotter::AddCurve(spaceFTHistoryGraph,
                                    Slab::DummyPointer(cutoffLine),
                                    PlotThemeManager::GetCurrent()->funcPlotStyles[0], "k cutoff");
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

        auto rebuiltHistory = Slab::New<R2toR::DiscreteFunction_CPU>(N, n, xMin, tMin, hx, ht);

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

        inverseDFTArtist->setFunction(rebuiltHistory);
    }

    void RtoRFourierPanel::computeTimeDFT(Real t_0, Real t_f) {
        fix N = simulationHistory->getN();
        fix xMin = simulationHistory->getDomain().xMin;
        fix dx = simulationHistory->getDomain().getLx()/N;

        fix Mₜ = simulationHistory->getM();
        fix dt = simulationHistory->getDomain().getLy()/Mₜ;
        fix Δt = t_f-t_0;
        fix __M = (Count)floor(Δt/dt);
        fix M = __M%2==0 ? __M : __M-1;
        fix m = M/2 + 1;

        fix dk = 2*M_PI/Δt;

        timeDFT = Slab::New<R2toR::DiscreteFunction_CPU>(N, m, xMin, 0, dx, dk);
        RtoR::DiscreteFunction_CPU tempSpace(M, .0, dk*M);

        fix j₀ = floor(t_0/dt);
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

        Str timeInterval = ToStr(t_0) + " ≤ t ≤ " + ToStr(t_f);
        timeDFTArtist->setFunction(timeDFT);
        timeDFTArtist->setLabel(Str("ℱₜ[ϕ](ω,x), ") + timeInterval);
    }

    void RtoRFourierPanel::setSimulationHistory(R2toR::DiscreteFunction_constptr simulationHistory,
                                                const R2toRFunctionArtist_ptr &simHistoryArtist) {
        RtoRPanel::setSimulationHistory(simulationHistory, simHistoryArtist);

        simulationHistoryGraph = Slab::New<PlottingWindow>();
        simulationHistoryGraph->addArtist(simulationHistoryArtist);
        simulationHistoryGraph->addArtist(inverseDFTArtist);
    }


} // Graphics
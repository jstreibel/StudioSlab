//
// Created by joao on 03/05/24.
//

#include "CorrelationsPanel.h"

#include "imgui.h"
#include "Math/Function/R2toR/Calc/R2toRDFT.h"
#include "Math/Function/R2toR/Model/R2toRDiscreteFunctionCPU.h"
#include "Math/Function/R2toC/R2toC_to_R2toR.h"

#include <memory>

namespace Graphics {

    fix N = 1200;
    fix M = 1538;

    fix L1 = 20.0;
    fix h = L1/N;
    fix L2 = h*M;
    auto func = new R2toR::DiscreteFunction_CPU(N, M, 0, 0, h, h);
    auto data = &func->getSpace().getHostData(false)[0];

    fix xCenter = 0;
    fix yCenter = 0;

    class SomeCurve : public RtoR2::ParametricCurve {
    public:
        Real2D operator()(Real t) const override {
            fix Δkₓ = 2*M_PI/L1 * N;
            fix Δkₜ = 2*M_PI/L2 * M;

            fix k_0x = 0;
            fix k_0y = 5;

            // return {0.5*Δkₓ*sin(8*t), 0.5*Δkₜ*t};

            /*
            if     (t<0.25) return {.1*Δkₓ*(1+t) , Δkₜ*0.25};
            else if(t<0.50) return {.1*Δkₓ*(1.25) , Δkₜ*0.25*(1.25-t)};
            else if(t<0.75) return {.1*Δkₓ*(2.00-t) , Δkₜ*0.25*( .25-t)};

            return {.1*Δkₓ, Δkₜ*0.25*( .25-t)};
             */

            // fix R = 2.0;
            // fix θ = 2*M_PI*t;
            // return {R*cos(θ)-k_0x, R*sin(θ)-k_0y};

            return {k_0x, k_0y};
        }
    };

    R2toR::DiscreteFunction_ptr Make_FFTW_TestFunc(int n_modes=150){

        SomeCurve someCurve;

        for(int i=0; i<N; ++i){
            for(int j=0; j<M; ++j) {
                fix x = L1 * i / (Real) N - xCenter;
                fix y = L2 * j / (Real) M - yCenter;
                fix r = sqrt(x * x + y * y);

                Real val=0.0;
                for (int n = 1; n <= n_modes; ++n) {
                    fix t = (n-1)/(Real)n_modes;

                    auto k = someCurve(t);
                    fix k1 = k.x;
                    fix k2 = k.y;

                    val += 1.0 / (n_modes) * sin(k1*x + k2*y);
                }

                data[i + j * N] = val;
            }
        }

        return Slab::Pointer<R2toR::DiscreteFunction>{func};
    }

    CorrelationsPanel::CorrelationsPanel(const NumericConfig &params, GUIWindow &guiWindow, RtoR::KGEnergy &hamiltonian)
    : RtoRPanel(params, guiWindow, hamiltonian,
                "Correlations",
                "panel for computing and visualizing correlations over simulation history data")
    , DFT2DGraph("Spacetime Fourier transform")
    , correlationGraph("Two-point correlation")
    {

    }

    void CorrelationsPanel::draw() {
        guiWindow.begin();

        if(ImGui::CollapsingHeader("ℱₜₓ and ⟨ϕ(t,x)ϕ(t′,x′)⟩")){
            static auto discardRedundant = false;
            // if(ImGui::Checkbox("Discard redundant modes", &discardRedundant)) {
            //     this->computeAll(discardRedundant);
            // }

            if(ImGui::Button("Compute"))
                this->computeAll(discardRedundant);
        }

        guiWindow.end();

        WindowPanel::draw();
    }

    void CorrelationsPanel::setSimulationHistory(R2toR::DiscreteFunction_constptr simulationHistory,
                                                 HistoryDisplay_ptr simHistoryGraph) {


        RtoRPanel::setSimulationHistory(simulationHistory, simHistoryGraph);

        addWindow(simulationHistoryGraph);
        addWindow(Slab::DummyPointer(correlationGraph));
        addWindow(Slab::DummyPointer(DFT2DGraph), true);
    }

    void CorrelationsPanel::computeAll(bool discardRedundantModes) {
        computeFullDFT2D(discardRedundantModes);
        computeTwoPointCorrelations();
    }

    void CorrelationsPanel::computeFullDFT2D(bool discardRedundantModes) {
        auto toFT = simulationHistory;
        // auto toFT = Make_FFTW_TestFunc();
        // correlationGraph.addFunction(toFT, "choopsy");

        if(discardRedundantModes) {
            dftFunction = R2toR::R2toRDFT::DFTReal_symmetric(*toFT);
        }
        else {
            dftFunction = R2toR::R2toRDFT::DFTReal(*toFT);
        }

        DFT2DGraph.removeFunction(ftAmplitudes);
        DFT2DGraph.removeFunction(ftPhases);
        DFT2DGraph.removeFunction(ftRealParts);
        DFT2DGraph.removeFunction(ftImagParts);

        ftAmplitudes = Math::Convert(dftFunction, Math::Magnitude);
        ftPhases     = Math::Convert(dftFunction, Math::Phase);
        ftRealParts  = Math::Convert(dftFunction, Math::Real);
        ftImagParts  = Math::Convert(dftFunction, Math::Imaginary);

        DFT2DGraph.addFunction(ftAmplitudes, "|ℱₜₓ[ϕ](ω,k)|");
        DFT2DGraph.addFunction(ftPhases,     "arg{ℱₜₓ[ϕ](ω,k)}");
        DFT2DGraph.addFunction(ftRealParts,  "ℜ{ℱₜₓ[ϕ](ω,k)}");
        DFT2DGraph.addFunction(ftImagParts,  "ℑ{ℱₜₓ[ϕ](ω,k)}");
    }

    void CorrelationsPanel::computeTwoPointCorrelations() {
        if(dftFunction == nullptr) return;

        if(1) {
            powerSpectrum = Math::Convert(dftFunction, Math::PowerSpectrum);

            auto invDFT    = R2toR::R2toRDFT::DFTComplex(*dftFunction, R2toR::R2toRDFT::InverseFourier);
            auto invPowDFT = R2toR::R2toRDFT::DFTReal(*powerSpectrum,
                                                      R2toR::R2toRDFT::InverseFourier,
                                                      R2toR::R2toRDFT::Auto, R2toR::R2toRDFT::Mangle);

            correlationGraph.addFunction(Math::Convert(invDFT, Math::Real), "choopsy doopsy");
            correlationGraph.addFunction(Math::Convert(invPowDFT, Math::Real), "choopsy doopsy power whoopsy");
        }
    }


} // Graphics
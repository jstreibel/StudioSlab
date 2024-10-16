//
// Created by joao on 03/05/24.
//

#include "CorrelationsPanel.h"

#include "3rdParty/ImGui.h"

#include "Math/Function/R2toR/Calc/R2toRDFT.h"
#include "Math/Function/R2toR/Model/R2toRNumericFunctionCPU.h"
#include "Math/Function/R2toC/R2toC_to_R2toR.h"
#include "Math/Function/RtoR2/ParametricCurve.h"

#include <memory>

namespace Slab::Models::KGRtoR {

    fix N = 1200;
    fix M = 1538;

    fix L1 = 20.0;
    fix h = L1/N;
    fix L2 = h*M;
    auto func = new R2toR::NumericFunction_CPU(N, M, 0, 0, h, h);
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

    R2toR::NumericFunction_ptr Make_FFTW_TestFunc(int n_modes=150){

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

        return Slab::Pointer<R2toR::NumericFunction>{func};
    }

    CorrelationsPanel::CorrelationsPanel(const Pointer<KGNumericConfig> &params, GUIWindow &guiWindow, KGEnergy &hamiltonian)
    : RtoRPanel(params, guiWindow, hamiltonian,
                "Correlations",
                "panel for computing and visualizing correlations over simulation history data")
    , DFT2DGraph("Spacetime Fourier transform")
    , correlationGraph("Two-point correlation")
    {

        ftAmplitudesArtist->setLabel("|ℱₜₓ[ϕ](ω,k)|");
        ftPhasesArtist->setLabel(   "arg{ℱₜₓ[ϕ](ω,k)}");
        ftRealPartsArtist->setLabel("ℜ{ℱₜₓ[ϕ](ω,k)}");
        ftImagPartsArtist->setLabel("ℑ{ℱₜₓ[ϕ](ω,k)}");

        DFT2DGraph.addArtist(ftAmplitudesArtist);
        DFT2DGraph.addArtist(ftPhasesArtist);
        DFT2DGraph.addArtist(ftRealPartsArtist);
        DFT2DGraph.addArtist(ftImagPartsArtist);

        DFT2DGraph.getAxisArtist().setHorizontalAxisLabel("k");
        DFT2DGraph.getAxisArtist().setVerticalAxisLabel("ω");

        correlationGraph.addArtist(twoPointCorrArtist);
        correlationGraph.getAxisArtist().setHorizontalAxisLabel("x");
        correlationGraph.getAxisArtist().setVerticalAxisLabel("t");
    }

    void CorrelationsPanel::draw() {        guiWindow.begin();

        if (ImGui::CollapsingHeader("ℱₜₓ and ⟨ϕ(t,x)ϕ(t′,x′)⟩")) {
            static auto discardRedundant = false;
            // if(ImGui::Checkbox("Discard redundant modes", &discardRedundant)) {
            //     this->computeAll(discardRedundant);
            // }

            if (ImGui::Button("Compute"))
                this->computeAll(discardRedundant);
        }

        guiWindow.end();

        WindowPanel::draw();
    }

    void CorrelationsPanel::setSimulationHistory(R2toR::NumericFunction_constptr simulationHistory,
                                                 const R2toRFunctionArtist_ptr &simHistoryArtist) {
        RtoRPanel::setSimulationHistory(simulationHistory, simHistoryArtist);

        auto simulationHistoryGraph = Slab::New<Plot2DWindow>();
        simulationHistoryGraph->addArtist(simulationHistoryArtist);
        addWindow(simulationHistoryGraph);
        addWindow(Slab::Naked(correlationGraph));
        addWindow(Slab::Naked(DFT2DGraph), true);
    }

    void CorrelationsPanel::computeAll(bool discardRedundantModes) {
        computeFullDFT2D(discardRedundantModes);
        computeTwoPointCorrelations();
    }

    void CorrelationsPanel::computeFullDFT2D(bool discardRedundantModes) {
        auto toFT = simulationHistory;
        // auto toFT = Make_FFTW_TestFunc();
        // correlationGraph.addRtoRFunction(toFT, "choopsy");

        if(discardRedundantModes)
            dftFunction = R2toR::R2toRDFT::DFTReal_symmetric(*toFT);
        else
            dftFunction = R2toR::R2toRDFT::DFTReal(*toFT);

        auto ftAmplitudes = Math::Convert(dftFunction, Math::Magnitude);
        auto ftPhases     = Math::Convert(dftFunction, Math::Phase);
        auto ftRealParts  = Math::Convert(dftFunction, Math::RealPart);
        auto ftImagParts  = Math::Convert(dftFunction, Math::ImaginaryPart);

        ftAmplitudesArtist->setFunction(ftAmplitudes);
        ftPhasesArtist    ->setFunction(ftPhases);
        ftRealPartsArtist ->setFunction(ftRealParts);
        ftImagPartsArtist ->setFunction(ftImagParts);
    }

    void CorrelationsPanel::computeTwoPointCorrelations() {
        if(dftFunction == nullptr) return;


        auto powerSpectrum = Math::Convert(dftFunction, Math::PowerSpectrum);

        auto invDFT    = R2toR::R2toRDFT::DFTComplex(*dftFunction, R2toR::R2toRDFT::InverseFourier);
        auto invPowDFT = R2toR::R2toRDFT::DFTReal(*powerSpectrum,
                                                  R2toR::R2toRDFT::InverseFourier,
                                                  R2toR::R2toRDFT::Auto, R2toR::R2toRDFT::Mangle);

        twoPointCorrArtist->setFunction(Math::Convert(invPowDFT, Math::RealPart));
        twoPointCorrArtist->setLabel("ℱ⁻¹[P], P≡|ℱ|²");
    }




} // Graphics
//
// Created by joao on 20/04/24.
//

#include "RtoRFourierPanel.h"

#include "Core/Controller/CommandLine/CommandLineInterfaceManager.h"

#include "Math/Function/R2toR/Calc/R2toRDFT.h"
#include "Math/Function/R2toC/R2toC_to_R2toR.h"

#include "Graphics/Plot2D/PlotThemeManager.h"
#include "3rdParty/ImGui.h"
#include "Graphics/Plot2D/Plotter.h"

namespace Slab::Models::KGRtoR {

    constexpr auto NoModeDiscard = false;

    RtoRFourierPanel::RtoRFourierPanel(const Pointer<KGNumericConfig> &params, KGEnergy &hamiltonian, FGUIWindow &guiWindow)
    : FRtoRPanel(params, guiWindow, hamiltonian, "ℝ↦ℝ Fourier panel", "Fourier analysis panel")
    , cutoffLine({kFilterCutoff, -10.0}, {kFilterCutoff, params->gett()+10.0})
    {
        kSpaceGraph  = Slab::New<FPlot2DWindow>("ℱₓ");
        ωSpaceGraph  = Slab::New<FPlot2DWindow>("ℱₜ");
        xSpaceGraph  = Slab::New<FPlot2DWindow>("ϕ(t,x)");
        ωkSpaceGraph = Slab::New<FPlot2DWindow>("ℱₜₓ");

        inv_kSpaceArtist->SetLabel("ℱₖ⁻¹(t, x)");
        // inverseDFTDisplay->addArtist(inverseDFTArtist);
        // inverseDFTDisplay->getAxisArtist().setHorizontalAxisLabel("x");
        // inverseDFTDisplay->getAxisArtist().setVerticalAxisLabel("t");

        // Custom ticks
        auto unit = Constants::π;
        kSpaceGraph->GetAxisArtist().setHorizontalUnit(unit);
        fix WaveNumber = FCommandLineInterfaceManager::getInstance().getParameter("harmonic");
        if(WaveNumber != nullptr) {
            Graphics::FAxisArtist::Ticks ticks;
            fix L = params->GetL();
            fix dk = 2*M_PI/L;
            fix k = dk * WaveNumber->getValueAs<DevFloat>();
            for (int n = 1; n < 20; ++n) {
                ticks.push_back(Graphics::FAxisArtist::Tick{(2 * n - 1) * k, unit((2 * n - 1) * k, 0)});
            }
            kSpaceGraph->GetAxisArtist().setHorizontalAxisTicks(ticks);
        }
        kSpaceGraph->GetAxisArtist().SetHorizontalAxisLabel("k");
        kSpaceGraph->GetAxisArtist().setVerticalAxisLabel("t");

        ωSpaceArtist->SetLabel("ℱₜ(ω, x)");
        ωSpaceGraph->AddArtist(ωSpaceArtist);
        ωSpaceGraph->GetAxisArtist().SetHorizontalAxisLabel("x");
        ωSpaceGraph->GetAxisArtist().setVerticalAxisLabel("ω");

        timeFilteredArtist->SetLabel("ϕ[t ∈ (t₀,tₑ)]");
        xSpaceGraph->AddArtist(timeFilteredArtist);
        xSpaceGraph->AddArtist(inv_kSpaceArtist);
        xSpaceGraph->AddArtist(twoPointCorrArtist);

        powerArtist     ->SetLabel("ℙ=|ℱₜₓ|²");
        amplitudesArtist->SetLabel("|ℱₜₓ|");
        phasesArtist    ->SetLabel("arg{ℱₜₓ}");
        realPartsArtist ->SetLabel("ℜ{ℱₜₓ}");
        imagPartsArtist ->SetLabel("ℑ{ℱₜₓ}");

        ωkSpaceGraph->AddArtist(powerArtist);
        ωkSpaceGraph->AddArtist(amplitudesArtist);
        ωkSpaceGraph->AddArtist(phasesArtist);
        ωkSpaceGraph->AddArtist(realPartsArtist);
        ωkSpaceGraph->AddArtist(imagPartsArtist);


        {
            auto shared_xMin = ωSpaceGraph->GetRegion().getReference_xMin();
            auto shared_xMax = ωSpaceGraph->GetRegion().getReference_xMax();
            auto shared_tMin = kSpaceGraph->GetRegion().getReference_yMin();
            auto shared_tMax = kSpaceGraph->GetRegion().getReference_yMax();

            auto shared_ωMin = ωSpaceGraph->GetRegion().getReference_yMin();
            auto shared_ωMax = ωSpaceGraph->GetRegion().getReference_yMax();
            auto shared_kMin = kSpaceGraph->GetRegion().getReference_xMin();
            auto shared_kMax = kSpaceGraph->GetRegion().getReference_xMax();

            xSpaceGraph->GetRegion().setReference_xMin(shared_xMin);
            xSpaceGraph->GetRegion().setReference_xMax(shared_xMax);
            xSpaceGraph->GetRegion().setReference_yMin(shared_tMin);
            xSpaceGraph->GetRegion().setReference_yMax(shared_tMax);

            ωkSpaceGraph->GetRegion().setReference_yMin(shared_ωMin);
            ωkSpaceGraph->GetRegion().setReference_yMax(shared_ωMax);
            ωkSpaceGraph->GetRegion().setReference_xMin(shared_kMin);
            ωkSpaceGraph->GetRegion().setReference_xMax(shared_kMax);
        }

        AddWindow(ωSpaceGraph);
        AddWindow(xSpaceGraph);
        AddWindow(ωkSpaceGraph, true);
        AddWindow(kSpaceGraph);

        arrangeWindows();
    }

    void RtoRFourierPanel::ImmediateDraw() {

        guiWindow.AddExternalDraw([this](){
            if (ImGui::CollapsingHeader("k-filter")) {
                //this->dftData
                fix kMax = M_PI / Params->geth();
                auto k = (float) kFilterCutoff;

                static bool needRefresh = false;
                static int selected = 0;
                static bool autoRefresh = false;
                if (ImGui::Checkbox("Auto##space_dft", &autoRefresh) && autoRefresh) needRefresh = true;

                if (ImGui::RadioButton("High-pass", &selected, 0) | ImGui::RadioButton("Low-pass", &selected, 1))
                    needRefresh = true;

                if (ImGui::SliderFloat("cutoff k", &k, 0.0, (float) kMax)) {
                    kFilterCutoff = k;
                    fix t = Params->gett();
                    cutoffLine.getx0() = {kFilterCutoff, -10.0};
                    cutoffLine.getr() = {0, 10.0 + t};

                    needRefresh = true;
                }

                if ((autoRefresh || ImGui::Button("Compute ℱₖ⁻¹")) && needRefresh) {
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

            if (ImGui::CollapsingHeader("t-filter, ℱₜ & ℱₜₓ")) {
                static auto t_0 = .0f;
                static fix tMax = (float) FRtoRPanel::Params->gett();
                static auto t_f = tMax;
                static auto auto_update_Ft = false;
                static auto auto_update_Ftx = false;

                ImGui::Checkbox("Auto ℱₜ##time_dft", &auto_update_Ft);
                ImGui::Checkbox("Auto ℱₜₓ##tx_dft", &auto_update_Ftx);

                ImGui::BeginDisabled(auto_update_Ft);
                if (ImGui::Button("Compute ℱₜ"))
                    computeTimeDFT(t_0, t_f);
                ImGui::EndDisabled();
                ImGui::BeginDisabled(auto_update_Ftx);
                if (ImGui::Button("Compute ℱₜₓ"))
                    computeAll(t_0, t_f);
                ImGui::EndDisabled();

                if (ImGui::SliderFloat("tₘᵢₙ", &t_0, .0f, t_f) | ImGui::SliderFloat("tₘₐₓ", &t_f, t_0, tMax)) {
                    if (auto_update_Ft) computeTimeDFT(t_0, t_f);
                    if (auto_update_Ftx) computeAll(t_0, t_f);
                }
            }
        });

        WindowPanel::ImmediateDraw();
    }

    void
    RtoRFourierPanel::SetSpaceFourierHistory(Pointer<const R2toR::FNumericFunction>
 sftHistory,
                                             const FDFTDataHistory &dftDataHistory,
                                             const R2toRFunctionArtist_ptr &dftFunctionArtist) {

        FRtoRPanel::SetSpaceFourierHistory(sftHistory, dftDataHistory, dftFunctionArtist);

        kSpaceGraph->AddArtist(dftFunctionArtist);
        // kSpaceGraph->addArtist(dftFunctionArtist->getColorBarArtist());

        Graphics::Plotter::AddCurve(kSpaceGraph,
                                    Slab::Naked(cutoffLine),
                                    PlotThemeManager::GetCurrent()->FuncPlotStyles[0], "k cutoff");
    }

    void RtoRFourierPanel::refreshInverseDFT(RtoR::DFTInverse::Filter *filter) {
        assert((sizeof(DevFloat)==sizeof(double)) && " make sure this code is compatible with fftw3");

        fix xMin = FRtoRPanel::Params->getxMin();
        fix L = FRtoRPanel::Params->GetL();
        fix N = (*dftData)[0].Result.modeCount();
        fix hx = L/N;

        fix tMin = (*dftData)[0].t;
        fix t = dftData->back().t;
        fix n = dftData->size();
        fix ht = (t-tMin)/n;

        auto rebuiltHistory = DataAlloc<R2toR::NumericFunction_CPU>("ℱ⁻¹[dft_data]" , N, n, xMin, tMin, hx, ht);

        int _n = 0;
        for(auto &data : *dftData){

            auto func = RtoR::DFTInverse::Compute(data.Result, xMin, L,
                                                  filter);

            auto *out = &rebuiltHistory->At(0, _n);
            auto *in = &func->getSpace().getHostData(true)[0];

            memcpy(out, in, N*sizeof(DevFloat));

            ++_n;
        }

        inv_kSpaceArtist->setFunction(rebuiltHistory);
    }

    void RtoRFourierPanel::computeTimeDFT(DevFloat t_0, DevFloat t_f) {
        fix N = simulationHistory->getN();
        fix xMin = simulationHistory->getDomain().xMin;
        fix dx = simulationHistory->getDomain().getLx()/N;

        fix Mₜ = simulationHistory->getM();
        fix dt = simulationHistory->getDomain().getLy()/Mₜ;
        fix Δt = t_f-t_0;
        fix __M = (CountType)floor(Δt/dt);
        fix M = __M%2==0 ? __M : __M-1;
        fix m = M/2 + 1;

        fix dk = 2*M_PI/Δt;

        ωSpace = DataAlloc<R2toR::NumericFunction_CPU>("ℱₓ[ϕ](ω)", N, m, xMin, 0, dx, dk);
        RtoR::NumericFunction_CPU tempSpace(M, .0, dk*M);

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

                ωSpace->At(i, k) = A;
            }
        }

        Str timeInterval = ToStr(t_0) + " ≤ t ≤ " + ToStr(t_f);
        ωSpaceArtist->setFunction(ωSpace);
        ωSpaceArtist->SetLabel(Str("ℱₜ[ϕ](ω,x), ") + timeInterval);
    }

    void RtoRFourierPanel::SetSimulationHistory(Pointer<const R2toR::FNumericFunction>
 simulationHistory,
                                                const R2toRFunctionArtist_ptr &simHistoryArtist) {
        FRtoRPanel::SetSimulationHistory(simulationHistory, simHistoryArtist);

        xSpaceGraph->AddArtist(simulationHistoryArtist, -10);
    }

    void RtoRFourierPanel::computeAll(DevFloat t_0, DevFloat t_f) {
        computeFullDFT2D(t_0, t_f, NoModeDiscard);
        computeTwoPointCorrelations();
    }

    void RtoRFourierPanel::computeFullDFT2D(DevFloat t_0, DevFloat t_f, bool discardRedundantModes) {
        auto toFT = FilterSpace(simulationHistory, t_0, t_f);
        timeFilteredArtist->setFunction(toFT);

        // auto toFT = Make_FFTW_TestFunc();
        // correlationGraph.addRtoRFunction(toFT, "choopsy");

        if(discardRedundantModes)
            dft2DFunction = R2toR::R2toRDFT::DFTReal_symmetric(*toFT);
        else
            dft2DFunction = R2toR::R2toRDFT::DFTReal(*toFT);

        auto ftAmplitudes = Math::Convert(dft2DFunction, Math::Magnitude);
        auto ftPhases     = Math::Convert(dft2DFunction, Math::Phase);
        auto ftRealParts  = Math::Convert(dft2DFunction, Math::RealPart);
        auto ftImagParts  = Math::Convert(dft2DFunction, Math::ImaginaryPart);

        amplitudesArtist->setFunction(ftAmplitudes);
        phasesArtist    ->setFunction(ftPhases);
        realPartsArtist ->setFunction(ftRealParts);
        imagPartsArtist ->setFunction(ftImagParts);
    }

    void RtoRFourierPanel::computeTwoPointCorrelations() {
        if(dft2DFunction == nullptr) return;

        auto powerSpectrum = Math::Convert(dft2DFunction, Math::PowerSpectrum);

        // auto invDFT    = R2toR::R2toRDFT::DFTComplex(*dft2DFunction, R2toR::R2toRDFT::InverseFourier);
        auto twoPtCorrFunction = R2toR::R2toRDFT::DFTReal(*powerSpectrum,
                                                  R2toR::R2toRDFT::InverseFourier,
                                                  R2toR::R2toRDFT::Auto, R2toR::R2toRDFT::Mangle);

        twoPointCorrArtist->setFunction(Math::Convert(twoPtCorrFunction, Math::RealPart));
        twoPointCorrArtist->SetLabel("ℱ⁻¹[P], P≡|ℱ|²");

        powerArtist->setFunction(powerSpectrum);
    }

    Pointer<R2toR::FNumericFunction>
    RtoRFourierPanel::FilterSpace(Pointer<const R2toR::FNumericFunction> func, DevFloat tMin, DevFloat tMax) {
        fix N = func->getN();
        fix xMin = func->getDomain().xMin;
        fix dx = func->getDomain().getLx()/N;

        fix Mₜ = func->getM();
        fix dt = func->getDomain().getLy()/Mₜ;
        fix Δt = tMax-tMin;
        fix __M = (CountType)floor(Δt/dt);
        fix M = __M%2==0 ? __M : __M-1;

        auto out = DataAlloc<R2toR::NumericFunction_CPU>(
                func->get_data_name() + " t∈(" + ToStr(tMin) + "," + ToStr(tMax) + ")", N, M, xMin, tMin, dx, Δt / (DevFloat)M);

        fix j₀ = floor(tMin/dt);

        for (auto i = 0; i < N; ++i) for (auto j = 0; j < M; ++j)
            out->At(i, j) = func->At(i, j₀+j);

        return out;
    }


} // Graphics

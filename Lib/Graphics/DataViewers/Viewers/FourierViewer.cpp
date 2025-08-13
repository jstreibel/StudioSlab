//
// Created by joao on 7/3/24.
//

#include "Graphics/Modules/ImGui/ImGuiModule.h"
#include "Core/Backend/BackendManager.h"

#include "3rdParty/ImGui.h"

// #include "Graphics/OpenGL/OpenGL.h"

#include "FourierViewer.h"

#include <utility>
#include "Math/Function/R2toR/Model/R2toRNumericFunctionCPU.h"
#include "Math/Function/R2toR/Calc/R2toRDFT.h"
#include "Math/Function/R2toC/R2toC_to_R2toR.h"
#include "Math/Function/RtoR/Model/RtoRNumericFunctionCPU.h"
#include "Graphics/Plot2D/Plotter.h"

namespace Slab::Graphics {

    constexpr auto KeepRedundantModes = false;

    FourierViewer::FourierViewer(const TPointer<FGUIWindow>& gui_window) : Viewer(gui_window)
    {
        auto ImGuiContextPtr = gui_window->GetGUIWindowContext();

        kSpaceGraph  = New<FPlot2DWindow>("ℱₓ", ImGuiContextPtr);
        ωSpaceGraph  = New<FPlot2DWindow>("ℱₜ", ImGuiContextPtr);
        xSpaceGraph  = New<FPlot2DWindow>("ϕ(t,x)", ImGuiContextPtr);
        ωkSpaceGraph = New<FPlot2DWindow>("ℱₜₓ", ImGuiContextPtr);

        inv_kSpaceArtist->SetLabel("ℱₖ⁻¹(t, x)");
        // inverseDFTDisplay->addArtist(inverseDFTArtist);
        // inverseDFTDisplay->getAxisArtist().setHorizontalAxisLabel("x");
        // inverseDFTDisplay->getAxisArtist().setVerticalAxisLabel("t");

        {
            kSpaceGraph->AddArtist(kSpace_powerArtist, 2);
            kSpaceGraph->AddArtist(kSpace_amplitudesArtist, 3);
            kSpaceGraph->AddArtist(kSpace_phasesArtist, 0);
            kSpaceGraph->AddArtist(kSpace_imagPartsArtist, 1);
            kSpaceGraph->AddArtist(kSpace_realPartsArtist, 1);

            kSpace_powerArtist      ->SetLabel("|ℱₖ|²");
            kSpace_amplitudesArtist ->SetLabel("|ℱₖ|");
            kSpace_phasesArtist     ->SetLabel("arg ℱₖ");
            kSpace_imagPartsArtist  ->SetLabel("ℑ ℱₖ");
            kSpace_realPartsArtist  ->SetLabel("ℜ ℱₖ");
        }

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
            fix shared_xMin = ωSpaceGraph->GetRegion().getReference_xMin();
            fix shared_xMax = ωSpaceGraph->GetRegion().getReference_xMax();
            fix shared_tMin = kSpaceGraph->GetRegion().getReference_yMin();
            fix shared_tMax = kSpaceGraph->GetRegion().getReference_yMax();

            fix shared_ωMin = ωSpaceGraph->GetRegion().getReference_yMin();
            fix shared_ωMax = ωSpaceGraph->GetRegion().getReference_yMax();
            fix shared_kMin = kSpaceGraph->GetRegion().getReference_xMin();
            fix shared_kMax = kSpaceGraph->GetRegion().getReference_xMax();

            xSpaceGraph->GetRegion().setReference_xMin(shared_xMin);
            xSpaceGraph->GetRegion().setReference_xMax(shared_xMax);
            xSpaceGraph->GetRegion().setReference_yMin(shared_tMin);
            xSpaceGraph->GetRegion().setReference_yMax(shared_tMax);

            ωkSpaceGraph->GetRegion().setReference_yMin(shared_ωMin);
            ωkSpaceGraph->GetRegion().setReference_yMax(shared_ωMax);
            ωkSpaceGraph->GetRegion().setReference_xMin(shared_kMin);
            ωkSpaceGraph->GetRegion().setReference_xMax(shared_kMax);
        }

        AddWindow(ωSpaceGraph, false, 0.5);
        AddWindow(xSpaceGraph);
        AddWindow(ωkSpaceGraph, true, 0.5);
        AddWindow(kSpaceGraph);

        ArrangeWindows();
    }

    void FourierViewer::ImmediateDraw(const FPlatformWindow& PlatformWindow) {
        auto function = getFunction();
        if(function == nullptr) return;

        gui_window->AddExternalDraw([this]() {
            auto function = getFunction();

            if (ImGui::CollapsingHeader("k-filter")) {
                //this->dftData
                fix kMax = M_PI / function->getSpace().getMetaData().geth(0);
                auto k = (float) kFilterCutoff;

                static bool needRefresh = false;
                static int selected = 0;
                static bool autoRefresh = false;
                if (ImGui::Checkbox("Auto##space_dft", &autoRefresh) && autoRefresh) needRefresh = true;

                if (ImGui::RadioButton("High-pass", &selected, 0) | ImGui::RadioButton("Low-pass", &selected, 1))
                    needRefresh = true;

                if (ImGui::SliderFloat("cutoff k", &k, 0.0, (float) kMax)) {
                    kFilterCutoff = k;
                    fix t_min = function->getDomain().yMin;
                    fix t_max = function->getDomain().yMax;
                    cutoffLine.getx0() = {kFilterCutoff, t_min - 10.0};
                    cutoffLine.getr() = {0, t_max + 10.0};

                    needRefresh = true;
                }

                if ((autoRefresh || ImGui::Button("Compute ℱₖ⁻¹")) && needRefresh) {
                    if (selected == 1) {
                        RtoR::DFTInverse::LowPass lowPass((int) kFilterCutoff);
                        refreshInverseDFT(&lowPass);
                    } else if (selected == 0) {
                        RtoR::DFTInverse::HighPass highPass((int) kFilterCutoff);
                        refreshInverseDFT(&highPass);
                    }

                    needRefresh = false;
                }
            }

            if (ImGui::CollapsingHeader("t-filter, ℱₜ & ℱₜₓ")) {
                fix dt = function->getSpace().getMetaData().geth(1);
                fix t_min = (float) function->getDomain().yMin;
                fix t_max = (float) function->getDomain().yMax;

                ImGui::Checkbox("Auto ℱₜ##time_dft", &auto_update_Ft);
                ImGui::Checkbox("Auto ℱₜₓ##tx_dft", &auto_update_Ftx);

                ImGui::BeginDisabled(auto_update_Ft);
                if (ImGui::Button("Compute ℱₜ"))
                    computeTimeDFT();
                ImGui::EndDisabled();
                ImGui::BeginDisabled(auto_update_Ftx);
                if (ImGui::Button("Compute ℱₜₓ"))
                    computeAll();
                ImGui::EndDisabled();

                if (ImGui::SliderFloat("tₘᵢₙ", &t0, t_min, t_max - Δt)
                    | ImGui::SliderFloat("Δt", &Δt, 10 * (float) dt, t_max - t0)) {
                    if (auto_update_Ft) computeTimeDFT();
                    if (auto_update_Ftx) computeAll();
                }
            }
        });

        WindowPanel::ImmediateDraw(PlatformWindow);
    }

    auto FourierViewer::FilterSpace(const TPointer<const R2toR::FNumericFunction>& func, DevFloat tMin,
                                    DevFloat tMax) -> TPointer<R2toR::FNumericFunction> {

        fix t_min = func->getDomain().yMin;
        fix t_max = func->getDomain().yMax;

        tMin = Common::max(tMin, t_min);
        tMax = Common::min(tMax, t_max);

        fix N = func->getN();
        fix xMin = func->getDomain().xMin;
        fix dx = func->getDomain().getLx()/N;

        fix Mₜ = func->getM();
        fix dt = func->getDomain().getLy()/Mₜ;
        fix Δt = tMax-tMin;
        fix test_M = static_cast<CountType>(floor(Δt / dt));
        fix M = test_M%2==0 ? test_M : test_M-1;

        auto out = DataAlloc<Math::R2toR::NumericFunction_CPU>(
                func->get_data_name() + " t∈(" + ToStr(tMin) + "," + ToStr(tMax) + ")",
                N, M, xMin, tMin, dx, Δt/static_cast<DevFloat>(M));

        fix j₀ = static_cast<UInt>(floor((tMin-t_min)/dt));

        for (auto i = 0; i < N; ++i) for (auto j = 0; j < M; ++j)
                out->At(i, j) = func->At(i, j₀+j);

        return out;
    }

    void FourierViewer::computeAll() {
        if(fix function = getFunction(); function == nullptr) return;

        computeFullDFT2D(KeepRedundantModes);
        computeTwoPointCorrelations();
    }

    void FourierViewer::computeFullDFT2D(bool discardRedundantModes) {
        const auto function = getFunction();

        if(function == nullptr) return;

        fix t_0 = static_cast<DevFloat>(t0);
        fix t_f = t_0 + static_cast<DevFloat>(Δt);

        auto toFT = FilterSpace(function, t_0, t_f);
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

    void FourierViewer::computeTwoPointCorrelations() {
        // if(function == nullptr) return;
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

    void FourierViewer::refreshInverseDFT(RtoR::DFTInverse::Filter *filter) {
        auto function = getFunction();

        if(function == nullptr) return;

        assert((sizeof(DevFloat)==sizeof(double)) && " make sure this code is compatible with fftw3");

        fix xMin = function->getDomain().xMin;
        fix L = function->getDomain().getLx();
        fix N = function->getN();
        fix M = function->getM();
        IN data = function->getSpace().getHostData(true);
        for(auto j=0; j<M; ++j) {

        }


        /*
        fix N = (*dftData)[0].result.modeCount();
        fix hx = L/N;

        fix tMin = (*dftData)[0].t;
        fix t = dftData->back().t;
        fix n = dftData->size();
        fix ht = (t-tMin)/n;

        auto rebuiltHistory = Slab::New<R2toR::NumericFunction_CPU>(N, n, xMin, tMin, hx, ht);

        int _n = 0;
        for(auto &data : *dftData){

            auto func = RtoR::DFTInverse::Compute(data.result, xMin, L,
                                                  filter);

            auto *out = &rebuiltHistory->At(0, _n);
            auto *in = &func->getSpace().getHostData(true)[0];

            memcpy(out, in, N*sizeof(DevFloat));

            ++_n;
        }

         inv_kSpaceArtist->setFunction(rebuiltHistory);
        */
    }

    void FourierViewer::computeTimeDFT() {
        auto function = getFunction();

        if(function == nullptr) return;

        DevFloat t_0 = (DevFloat)t0;
        DevFloat t_f = t_0 + (DevFloat)Δt;

        fix t_min = function->getDomain().yMin;

        t_0 = Common::max(t_0, t_min);
        t_f = Common::min(t_f, function->getDomain().yMax);

        fix N = function->getN();
        fix xMin = function->getDomain().xMin;
        fix dx = function->getDomain().getLx()/N;

        fix Mₜ = function->getM();
        fix dt = function->getDomain().getLy()/Mₜ;
        fix Δt = t_f-t_0;
        fix __M = (CountType)floor(Δt/dt);
        fix M = __M%2==0 ? __M : __M-1;
        fix m = M/2 + 1;

        fix dk = 2*M_PI/Δt;

        ωSpace = DataAlloc<R2toR::NumericFunction_CPU>("ω-space", N, m, xMin, 0, dx, dk);
        RtoR::NumericFunction_CPU tempSpace(M, .0, dk*M);

        fix j₀ = floor((t_0-t_min)/dt);
        for(auto i=0; i<N; ++i){
            auto &spaceData_temp = tempSpace.getSpace().getHostData(false);

            for(auto j=0; j<M; ++j) spaceData_temp[j] = function->At(i, j₀+j);

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



    void FourierViewer::SetFunction(const FourierViewer::Function func) {
        Viewer::SetFunction(func);

        fix function = getFunction();

        fix t_min = static_cast<float>(function->getDomain().yMin);
        fix t_max = static_cast<float>(function->getDomain().yMax);
        t0 = t_min;
        Δt = t_max-t_min;

        cutoffLine = RtoR2::StraightLine({kFilterCutoff, function->getDomain().yMin-10.0},
                                         {kFilterCutoff, function->getDomain().yMax+10.0});

        if(function_artist == nullptr) {
            function_artist = Plotter::AddR2toRFunction(xSpaceGraph, function, "ϕ");
            function_artist->SetAffectGraphRanges(true);
        } else {
            function_artist->setFunction(function);
        }

        xSpaceGraph->ReviewGraphRanges();

        if (is_Ft_auto_updating()) computeTimeDFT();
        if (is_Ftx_auto_updating()) computeAll();

        computeSpaceDFT();
    }

    bool FourierViewer::is_Ft_auto_updating() const {
        return auto_update_Ft;
    }

    bool FourierViewer::is_Ftx_auto_updating() const {
        return auto_update_Ftx;
    }

    void FourierViewer::computeSpaceDFT() {
        auto function = getFunction();

        if(function == nullptr) return;

        auto space_dft = R2toR::R2toRDFT::SpaceDFTReal(*function);

        auto power      = Math::Convert(space_dft, ER2toC_to_R2toR_Mode::PowerSpectrum);
        auto amplitudes = Math::Convert(space_dft, ER2toC_to_R2toR_Mode::Magnitude);
        auto phases     = Math::Convert(space_dft, ER2toC_to_R2toR_Mode::Phase);
        auto realParts  = Math::Convert(space_dft, ER2toC_to_R2toR_Mode::RealPart);
        auto imagParts  = Math::Convert(space_dft, ER2toC_to_R2toR_Mode::ImaginaryPart);

        kSpace_powerArtist->setFunction(power);
        kSpace_amplitudesArtist->setFunction(amplitudes);
        kSpace_phasesArtist->setFunction(phases);
        kSpace_realPartsArtist->setFunction(realParts);
        kSpace_imagPartsArtist->setFunction(imagParts);


    }

    Str FourierViewer::GetName() const {
        return "Fourier transforms viewer";
    }


} // Studios::Fields
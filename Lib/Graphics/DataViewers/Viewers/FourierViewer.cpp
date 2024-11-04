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

    FourierViewer::FourierViewer(Pointer<GUIWindow> gui_window) : Viewer(std::move(gui_window))
    {
        kSpaceGraph  = New<Plot2DWindow>("ℱₓ", this->gui_window->GetGUIContext());
        ωSpaceGraph  = New<Plot2DWindow>("ℱₜ", this->gui_window->GetGUIContext());
        xSpaceGraph  = New<Plot2DWindow>("ϕ(t,x)", this->gui_window->GetGUIContext());
        ωkSpaceGraph = New<Plot2DWindow>("ℱₜₓ", this->gui_window->GetGUIContext());

        inv_kSpaceArtist->setLabel("ℱₖ⁻¹(t, x)");
        // inverseDFTDisplay->addArtist(inverseDFTArtist);
        // inverseDFTDisplay->getAxisArtist().setHorizontalAxisLabel("x");
        // inverseDFTDisplay->getAxisArtist().setVerticalAxisLabel("t");

        {
            kSpaceGraph->addArtist(kSpace_powerArtist, 2);
            kSpaceGraph->addArtist(kSpace_amplitudesArtist, 3);
            kSpaceGraph->addArtist(kSpace_phasesArtist, 0);
            kSpaceGraph->addArtist(kSpace_imagPartsArtist, 1);
            kSpaceGraph->addArtist(kSpace_realPartsArtist, 1);

            kSpace_powerArtist      ->setLabel("|ℱₖ|²");
            kSpace_amplitudesArtist ->setLabel("|ℱₖ|");
            kSpace_phasesArtist     ->setLabel("arg ℱₖ");
            kSpace_imagPartsArtist  ->setLabel("ℑ ℱₖ");
            kSpace_realPartsArtist  ->setLabel("ℜ ℱₖ");
        }

        ωSpaceArtist->setLabel("ℱₜ(ω, x)");
        ωSpaceGraph->addArtist(ωSpaceArtist);
        ωSpaceGraph->getAxisArtist().setHorizontalAxisLabel("x");
        ωSpaceGraph->getAxisArtist().setVerticalAxisLabel("ω");

        timeFilteredArtist->setLabel("ϕ[t ∈ (t₀,tₑ)]");
        xSpaceGraph->addArtist(timeFilteredArtist);
        xSpaceGraph->addArtist(inv_kSpaceArtist);
        xSpaceGraph->addArtist(twoPointCorrArtist);

        powerArtist     ->setLabel("ℙ=|ℱₜₓ|²");
        amplitudesArtist->setLabel("|ℱₜₓ|");
        phasesArtist    ->setLabel("arg{ℱₜₓ}");
        realPartsArtist ->setLabel("ℜ{ℱₜₓ}");
        imagPartsArtist ->setLabel("ℑ{ℱₜₓ}");

        ωkSpaceGraph->addArtist(powerArtist);
        ωkSpaceGraph->addArtist(amplitudesArtist);
        ωkSpaceGraph->addArtist(phasesArtist);
        ωkSpaceGraph->addArtist(realPartsArtist);
        ωkSpaceGraph->addArtist(imagPartsArtist);


        {
            auto shared_xMin = ωSpaceGraph->getRegion().getReference_xMin();
            auto shared_xMax = ωSpaceGraph->getRegion().getReference_xMax();
            auto shared_tMin = kSpaceGraph->getRegion().getReference_yMin();
            auto shared_tMax = kSpaceGraph->getRegion().getReference_yMax();

            auto shared_ωMin = ωSpaceGraph->getRegion().getReference_yMin();
            auto shared_ωMax = ωSpaceGraph->getRegion().getReference_yMax();
            auto shared_kMin = kSpaceGraph->getRegion().getReference_xMin();
            auto shared_kMax = kSpaceGraph->getRegion().getReference_xMax();

            xSpaceGraph->getRegion().setReference_xMin(shared_xMin);
            xSpaceGraph->getRegion().setReference_xMax(shared_xMax);
            xSpaceGraph->getRegion().setReference_yMin(shared_tMin);
            xSpaceGraph->getRegion().setReference_yMax(shared_tMax);

            ωkSpaceGraph->getRegion().setReference_yMin(shared_ωMin);
            ωkSpaceGraph->getRegion().setReference_yMax(shared_ωMax);
            ωkSpaceGraph->getRegion().setReference_xMin(shared_kMin);
            ωkSpaceGraph->getRegion().setReference_xMax(shared_kMax);
        }

        addWindow(ωSpaceGraph, false, 0.5);
        addWindow(xSpaceGraph);
        addWindow(ωkSpaceGraph, true, 0.5);
        addWindow(kSpaceGraph);

        arrangeWindows();
    }

    void FourierViewer::draw() {
        auto function = getFunction();
        if(function == nullptr) return;

        beginGUI();

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

        endGUI();

        WindowPanel::draw();
    }

    auto FourierViewer::FilterSpace(const Pointer<const R2toR::NumericFunction>& func, Real t_0,
                                    Real t_f) -> Pointer<R2toR::NumericFunction> {

        fix t_min = func->getDomain().yMin;
        fix t_max = func->getDomain().yMax;

        t_0 = Common::max(t_0, t_min);
        t_f = Common::min(t_f, t_max);

        fix N = func->getN();
        fix xMin = func->getDomain().xMin;
        fix dx = func->getDomain().getLx()/N;

        fix Mₜ = func->getM();
        fix dt = func->getDomain().getLy()/Mₜ;
        fix Δt = t_f-t_0;
        fix test_M = (Count)floor(Δt/dt);
        fix M = test_M%2==0 ? test_M : test_M-1;

        auto out = DataAlloc<Math::R2toR::NumericFunction_CPU>(
                func->get_data_name() + " t∈(" + ToStr(t_0) + "," + ToStr(t_f) + ")",
                N, M, xMin, t_0, dx, Δt/(Real)M);

        fix j₀ = floor((t_0-t_min)/dt);

        for (auto i = 0; i < N; ++i) for (auto j = 0; j < M; ++j)
                out->At(i, j) = func->At(i, j₀+j);

        return out;
    }

    void FourierViewer::computeAll() {
        auto function = getFunction();
        if(function == nullptr) return;

        computeFullDFT2D(KeepRedundantModes);
        computeTwoPointCorrelations();
    }

    void FourierViewer::computeFullDFT2D(bool discardRedundantModes) {
        auto function = getFunction();

        if(function == nullptr) return;

        Real t_0 = (Real)t0;
        Real t_f = t_0 + (Real)Δt;

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
        twoPointCorrArtist->setLabel("ℱ⁻¹[P], P≡|ℱ|²");

        powerArtist->setFunction(powerSpectrum);

    }

    void FourierViewer::refreshInverseDFT(RtoR::DFTInverse::Filter *filter) {
        auto function = getFunction();

        if(function == nullptr) return;

        assert((sizeof(Real)==sizeof(double)) && " make sure this code is compatible with fftw3");

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

            memcpy(out, in, N*sizeof(Real));

            ++_n;
        }

         inv_kSpaceArtist->setFunction(rebuiltHistory);
        */
    }

    void FourierViewer::computeTimeDFT() {
        auto function = getFunction();

        if(function == nullptr) return;

        Real t_0 = (Real)t0;
        Real t_f = t_0 + (Real)Δt;

        fix t_min = function->getDomain().yMin;

        t_0 = Common::max(t_0, t_min);
        t_f = Common::min(t_f, function->getDomain().yMax);

        fix N = function->getN();
        fix xMin = function->getDomain().xMin;
        fix dx = function->getDomain().getLx()/N;

        fix Mₜ = function->getM();
        fix dt = function->getDomain().getLy()/Mₜ;
        fix Δt = t_f-t_0;
        fix __M = (Count)floor(Δt/dt);
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
        ωSpaceArtist->setLabel(Str("ℱₜ[ϕ](ω,x), ") + timeInterval);
    }



    void FourierViewer::setFunction(FourierViewer::Function func) {
        Viewer::setFunction(func);

        auto function = getFunction();

        fix t_min = (float)function->getDomain().yMin;
        fix t_max = (float)function->getDomain().yMax;
        t0 = t_min;
        Δt = t_max-t_min;

        cutoffLine = RtoR2::StraightLine({kFilterCutoff, function->getDomain().yMin-10.0},
                                         {kFilterCutoff, function->getDomain().yMax+10.0});

        if(function_artist == nullptr) {
            function_artist = Graphics::Plotter::AddR2toRFunction(xSpaceGraph, function, "ϕ");
            function_artist->setAffectGraphRanges(true);
        } else {
            function_artist->setFunction(function);
        }

        xSpaceGraph->reviewGraphRanges();

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

        auto power      = Math::Convert(space_dft, R2toC_to_R2toR_Mode::PowerSpectrum);
        auto amplitudes = Math::Convert(space_dft, R2toC_to_R2toR_Mode::Magnitude);
        auto phases     = Math::Convert(space_dft, R2toC_to_R2toR_Mode::Phase);
        auto realParts  = Math::Convert(space_dft, R2toC_to_R2toR_Mode::RealPart);
        auto imagParts  = Math::Convert(space_dft, R2toC_to_R2toR_Mode::ImaginaryPart);

        kSpace_powerArtist->setFunction(power);
        kSpace_amplitudesArtist->setFunction(amplitudes);
        kSpace_phasesArtist->setFunction(phases);
        kSpace_realPartsArtist->setFunction(realParts);
        kSpace_imagPartsArtist->setFunction(imagParts);
    }

    Str FourierViewer::getName() const {
        return "Fourier transforms viewer";
    }


} // Studios::Fields
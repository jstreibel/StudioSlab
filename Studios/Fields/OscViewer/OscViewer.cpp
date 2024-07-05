//
// Created by joao on 7/3/24.
//

#include "Core/Backend/Modules/ImGui/ImGuiModule.h"
#include "Core/Backend/BackendManager.h"

#include "3rdParty/ImGui.h"

// #include "Graphics/OpenGL/OpenGL.h"

#include "OscViewer.h"
#include "Math/Function/R2toR/Model/R2toRNumericFunctionCPU.h"
#include "Math/Function/R2toR/Calc/R2toRDFT.h"
#include "Math/Function/R2toC/R2toC_to_R2toR.h"
#include "Math/Function/RtoR/Model/RtoRNumericFunctionCPU.h"
#include "Graphics/Graph/Plotter.h"

#include <utility>

namespace Studios::Fields {

    constexpr auto KeepRedundantModes = false;

    OscViewer::OscViewer() : WindowPanel(Flags::HasMainMenu)
    {
        inv_kSpaceArtist->setLabel("ℱₖ⁻¹(t, x)");
        // inverseDFTDisplay->addArtist(inverseDFTArtist);
        // inverseDFTDisplay->getAxisArtist().setHorizontalAxisLabel("x");
        // inverseDFTDisplay->getAxisArtist().setVerticalAxisLabel("t");

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

        fix gui_window_width = 0.125f;
        fix other_width = (1-gui_window_width)/2;
        addWindow(Naked(guiWindow));
        addWindow(ωSpaceGraph, true, other_width);
        addWindow(xSpaceGraph);
        addWindow(ωkSpaceGraph, true, other_width);
        addWindow(kSpaceGraph);

        arrangeWindows();
    }

    void OscViewer::draw() {
        if(function == nullptr) return;

        guiWindow.begin();

        if(ImGui::CollapsingHeader("k-filter")){
            //this->dftData
            fix kMax = M_PI/function->getSpace().getMetaData().geth(0);
            auto k = (float)kFilterCutoff;

            static bool needRefresh = false;
            static int selected = 0;
            static bool autoRefresh = false;
            if(ImGui::Checkbox("Auto##space_dft", &autoRefresh) && autoRefresh) needRefresh = true;

            if(ImGui::RadioButton("High-pass", &selected, 0) | ImGui::RadioButton("Low-pass", &selected, 1))
                needRefresh = true;

            if(ImGui::SliderFloat("cutoff k", &k, 0.0, (float)kMax)){
                kFilterCutoff = k;
                fix t_min = function->getDomain().yMin;
                fix t_max = function->getDomain().yMax;
                cutoffLine.getx0() = {kFilterCutoff, t_min-10.0};
                cutoffLine.getr() = {0, t_max+10.0};

                needRefresh = true;
            }

            if((autoRefresh || ImGui::Button("Compute ℱₖ⁻¹")) && needRefresh) {
                if (selected == 1) {
                    RtoR::DFTInverse::LowPass lowPass((int)kFilterCutoff);
                    refreshInverseDFT(&lowPass);
                } else if (selected == 0) {
                    RtoR::DFTInverse::HighPass highPass((int)kFilterCutoff);
                    refreshInverseDFT(&highPass);
                }

                needRefresh = false;
            }
        }

        if(ImGui::CollapsingHeader("t-filter, ℱₜ & ℱₜₓ")) {
            static fix t_min =(float)function->getDomain().yMin;
            static fix t_max =(float)function->getDomain().yMax;
            static auto t_0 = t_min;
            static auto t_f = t_max;
            static auto auto_update_Ft = false;
            static auto auto_update_Ftx = false;

            ImGui::Checkbox("Auto ℱₜ##time_dft", &auto_update_Ft);
            ImGui::Checkbox("Auto ℱₜₓ##tx_dft", &auto_update_Ftx);

            ImGui::BeginDisabled(auto_update_Ft);
            if(ImGui::Button("Compute ℱₜ"))
                computeTimeDFT(t_0, t_f);
            ImGui::EndDisabled();
            ImGui::BeginDisabled(auto_update_Ftx);
            if(ImGui::Button("Compute ℱₜₓ"))
                computeAll(t_0, t_f);
            ImGui::EndDisabled();

            if(ImGui::SliderFloat("tₘᵢₙ", &t_0, .0f, t_f) | ImGui::SliderFloat("tₘₐₓ", &t_f, t_0, t_max)) {
                if (auto_update_Ft) computeTimeDFT(t_0, t_f);
                if (auto_update_Ftx) computeAll(t_0, t_f);
            }
        }

        guiWindow.end();

        WindowPanel::draw();
    }

    auto OscViewer::FilterSpace(const Pointer<const R2toR::NumericFunction>& func, Real tMin,
                                Real tMax) -> Pointer<R2toR::NumericFunction> {

        fix N = func->getN();
        fix xMin = func->getDomain().xMin;
        fix dx = func->getDomain().getLx()/N;

        fix Mₜ = func->getM();
        fix dt = func->getDomain().getLy()/Mₜ;
        fix Δt = tMax-tMin;
        fix test_M = (Count)floor(Δt/dt);
        fix M = test_M%2==0 ? test_M : test_M-1;

        auto out = New<Slab::Math::R2toR::NumericFunction_CPU>(N, M, xMin, tMin, dx, Δt/(Real)M);

        fix j₀ = floor(tMin/dt);

        for (auto i = 0; i < N; ++i) for (auto j = 0; j < M; ++j)
                out->At(i, j) = func->At(i, j₀+j);

        return out;
    }

    void OscViewer::computeAll(Real t_0, Real t_f) {
        if(function == nullptr) return;

        computeFullDFT2D(t_0, t_f, KeepRedundantModes);
        computeTwoPointCorrelations();
    }

    void OscViewer::computeFullDFT2D(Real t_0, Real t_f, bool discardRedundantModes) {
        if(function == nullptr) return;

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

    void OscViewer::computeTwoPointCorrelations() {
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

    void OscViewer::refreshInverseDFT(RtoR::DFTInverse::Filter *filter) {
        if(function == nullptr) return;

        Core::Log::Warning() << __PRETTY_FUNCTION__ << " not implemented.";
        /*
        assert((sizeof(Real)==sizeof(double)) && " make sure this code is compatible with fftw3");

        fix xMin = function->getDomain().xMin;
        fix L = function->getDomain().getLx();
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

    void OscViewer::computeTimeDFT(Real t_0, Real t_f) {
        if(function == nullptr) return;

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

        ωSpace = Slab::New<R2toR::NumericFunction_CPU>(N, m, xMin, 0, dx, dk);
        RtoR::NumericFunction_CPU tempSpace(M, .0, dk*M);

        fix j₀ = floor(t_0/dt);
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



    void OscViewer::setFunction(OscViewer::Function func) {
        function = std::move(func);

        cutoffLine = RtoR2::StraightLine({kFilterCutoff, function->getDomain().yMin-10.0},
                                         {kFilterCutoff, function->getDomain().yMax+10.0});

        auto artist = Graphics::Plotter::AddR2toRFunction(xSpaceGraph, function, "ϕ");
        artist->setAffectGraphRanges(true);
        xSpaceGraph->reviewGraphRanges();
    }


} // Studios::Fields
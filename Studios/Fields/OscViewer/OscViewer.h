//
// Created by joao on 7/3/24.
//

#ifndef STUDIOSLAB_OSCVIEWER_H
#define STUDIOSLAB_OSCVIEWER_H

#include "Graphics/Window/WindowContainer/WindowPanel.h"
#include "Graphics/Window/GUIWindow.h"
#include "Graphics/Graph/PlottingWindow.h"
#include "Graphics/Graph/Artists/R2toRFunctionArtist.h"

#include "Math/Function/RtoR/Operations/DFTInverse.h"
#include "Math/Function/R2toR/Model/R2toRNumericFunction.h"
#include "Math/Function/R2toC/R2toCNumericFunction.h"
#include "Math/Function/RtoR2/StraightLine.h"

namespace Studios::Fields {

    using namespace Slab;
    using namespace Slab::Graphics;

    class OscViewer : public WindowPanel {
        using Function = Slab::Pointer<Slab::Math::R2toR::NumericFunction>;

        Function function;
        R2toRFunctionArtist_ptr function_artist;

        Pointer<PlottingWindow> kSpaceGraph  = Slab::New<PlottingWindow>("ℱₓ");
        Pointer<PlottingWindow> ωSpaceGraph  = Slab::New<PlottingWindow>("ℱₜ");
        Pointer<PlottingWindow> xSpaceGraph  = Slab::New<PlottingWindow>("ϕ(t,x)");
        Pointer<PlottingWindow> ωkSpaceGraph = Slab::New<PlottingWindow>("ℱₜₓ");

        Pointer<R2toRFunctionArtist> inv_kSpaceArtist = Slab::New<R2toRFunctionArtist>();
        Pointer<R2toR::NumericFunction> inv_kSpace;

        Pointer<R2toRFunctionArtist> ωSpaceArtist = Slab::New<R2toRFunctionArtist>();
        Pointer<R2toR::NumericFunction> ωSpace;


        Pointer<R2toRFunctionArtist> twoPointCorrArtist = Slab::New<R2toRFunctionArtist>();
        Pointer<R2toRFunctionArtist> timeFilteredArtist = Slab::New<R2toRFunctionArtist>();


        Pointer<R2toRFunctionArtist> powerArtist        = Slab::New<R2toRFunctionArtist>();
        Pointer<R2toRFunctionArtist> amplitudesArtist   = Slab::New<R2toRFunctionArtist>();
        Pointer<R2toRFunctionArtist> phasesArtist       = Slab::New<R2toRFunctionArtist>();
        Pointer<R2toRFunctionArtist> realPartsArtist    = Slab::New<R2toRFunctionArtist>();
        Pointer<R2toRFunctionArtist> imagPartsArtist    = Slab::New<R2toRFunctionArtist>();
        Pointer<R2toC::NumericFunction> dft2DFunction;

        static auto FilterSpace(const Pointer<const R2toR::NumericFunction>& func, Real tMin, Real tMax) -> Pointer<R2toR::NumericFunction>;


        bool auto_update_Ft = false;
        bool auto_update_Ftx = false;
        float t0;
        float Δt;
    protected:
        void computeAll();
        void computeFullDFT2D(bool discardRedundantModes);
        void computeTwoPointCorrelations();


    private:
        Real kFilterCutoff = 0.0;
        RtoR2::StraightLine cutoffLine;
    protected:
        void refreshInverseDFT(RtoR::DFTInverse::Filter *filter);
        void computeTimeDFT();

        
    protected:
        bool is_Ft_auto_updating() const;
        bool is_Ftx_auto_updating() const;

        Graphics::GUIWindow guiWindow;

    public:
        explicit OscViewer();

        void setFunction(Function func);

        void draw() override;
    };

} // Studios::Fields

#endif //STUDIOSLAB_OSCVIEWER_H

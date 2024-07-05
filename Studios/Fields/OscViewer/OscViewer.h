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

        Graphics::GUIWindow guiWindow;

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

        void computeAll(Real t_0, Real t_f);
        void computeFullDFT2D(Real t_0, Real t_f, bool discardRedundantModes);
        void computeTwoPointCorrelations();

        Real kFilterCutoff = 0.0;
        RtoR2::StraightLine cutoffLine;
        void refreshInverseDFT(RtoR::DFTInverse::Filter *filter);
        void computeTimeDFT(Real tMin, Real tMax);

    public:
        explicit OscViewer();

        void setFunction(Function func);

        void draw() override;
    };

} // Studios::Fields

#endif //STUDIOSLAB_OSCVIEWER_H

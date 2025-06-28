//
// Created by joao on 7/3/24.
//

#ifndef STUDIOSLAB_FOURIERVIEWER_H
#define STUDIOSLAB_FOURIERVIEWER_H

#include "Graphics/Window/WindowContainer/WindowPanel.h"
#include "Graphics/Window/GUIWindow.h"
#include "Graphics/Plot2D/Plot2DWindow.h"
#include "Graphics/Plot2D/Artists/R2toRFunctionArtist.h"

#include "Math/Function/RtoR/Operations/DFTInverse.h"
#include "Math/Function/R2toR/Model/R2toRNumericFunction.h"
#include "Math/Function/R2toC/R2toCNumericFunction.h"
#include "Math/Function/RtoR2/StraightLine.h"
#include "Viewer.h"

namespace Slab::Graphics{

    class FourierViewer : public Viewer {
        using Function = Pointer<Math::R2toR::NumericFunction>;

        R2toRFunctionArtist_ptr function_artist;

        Pointer<Plot2DWindow> kSpaceGraph;
        Pointer<Plot2DWindow> ωSpaceGraph;
        Pointer<Plot2DWindow> xSpaceGraph;
        Pointer<Plot2DWindow> ωkSpaceGraph;

        Pointer<R2toRFunctionArtist> kSpace_powerArtist        = New<R2toRFunctionArtist>();
        Pointer<R2toRFunctionArtist> kSpace_amplitudesArtist   = New<R2toRFunctionArtist>();
        Pointer<R2toRFunctionArtist> kSpace_phasesArtist       = New<R2toRFunctionArtist>();
        Pointer<R2toRFunctionArtist> kSpace_realPartsArtist    = New<R2toRFunctionArtist>();
        Pointer<R2toRFunctionArtist> kSpace_imagPartsArtist    = New<R2toRFunctionArtist>();
        Pointer<R2toRFunctionArtist> kSpace_Ak2                = New<R2toRFunctionArtist>();

        Pointer<R2toRFunctionArtist> inv_kSpaceArtist = New<R2toRFunctionArtist>();
        Pointer<R2toR::NumericFunction> inv_kSpace;

        Pointer<R2toRFunctionArtist> ωSpaceArtist = New<R2toRFunctionArtist>();
        Pointer<R2toR::NumericFunction> ωSpace;


        Pointer<R2toRFunctionArtist> twoPointCorrArtist = New<R2toRFunctionArtist>();
        Pointer<R2toRFunctionArtist> timeFilteredArtist = New<R2toRFunctionArtist>();


        Pointer<R2toRFunctionArtist> powerArtist        = New<R2toRFunctionArtist>();
        Pointer<R2toRFunctionArtist> amplitudesArtist   = New<R2toRFunctionArtist>();
        Pointer<R2toRFunctionArtist> phasesArtist       = New<R2toRFunctionArtist>();
        Pointer<R2toRFunctionArtist> realPartsArtist    = New<R2toRFunctionArtist>();
        Pointer<R2toRFunctionArtist> imagPartsArtist    = New<R2toRFunctionArtist>();
        Pointer<R2toC::NumericFunction> dft2DFunction;

        bool auto_update_Ft = false;
        bool auto_update_Ftx = false;
        float t0=0.f;
        float Δt=1.f;
    protected:
        void computeAll();
        void computeFullDFT2D(bool discardRedundantModes);
        void computeTwoPointCorrelations();


    private:
        DevFloat kFilterCutoff = 0.0;
        RtoR2::StraightLine cutoffLine;
    protected:
        void refreshInverseDFT(RtoR::DFTInverse::Filter *filter);
        void computeTimeDFT();

        void computeSpaceDFT();

    protected:
        bool is_Ft_auto_updating() const;
        bool is_Ftx_auto_updating() const;

    public:
        static auto
        FilterSpace(const Pointer<const R2toR::NumericFunction>& func, DevFloat tMin, DevFloat tMax)
        -> Pointer<R2toR::NumericFunction>;

        Str getName() const override;

        explicit FourierViewer(Pointer<GUIWindow>);

        void setFunction(Function func) override;

        void draw() override;
    };

} // Studios::Fields

#endif //STUDIOSLAB_FOURIERVIEWER_H

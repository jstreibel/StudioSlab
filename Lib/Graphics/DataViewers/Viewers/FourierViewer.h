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
        using Function = TPointer<Math::R2toR::FNumericFunction>;

        R2toRFunctionArtist_ptr function_artist;

        TPointer<FPlot2DWindow> kSpaceGraph;
        TPointer<FPlot2DWindow> ωSpaceGraph;
        TPointer<FPlot2DWindow> xSpaceGraph;
        TPointer<FPlot2DWindow> ωkSpaceGraph;

        TPointer<R2toRFunctionArtist> kSpace_powerArtist        = New<R2toRFunctionArtist>();
        TPointer<R2toRFunctionArtist> kSpace_amplitudesArtist   = New<R2toRFunctionArtist>();
        TPointer<R2toRFunctionArtist> kSpace_phasesArtist       = New<R2toRFunctionArtist>();
        TPointer<R2toRFunctionArtist> kSpace_realPartsArtist    = New<R2toRFunctionArtist>();
        TPointer<R2toRFunctionArtist> kSpace_imagPartsArtist    = New<R2toRFunctionArtist>();
        TPointer<R2toRFunctionArtist> kSpace_Ak2                = New<R2toRFunctionArtist>();

        TPointer<R2toRFunctionArtist> inv_kSpaceArtist = New<R2toRFunctionArtist>();
        TPointer<R2toR::FNumericFunction> inv_kSpace;

        TPointer<R2toRFunctionArtist> ωSpaceArtist = New<R2toRFunctionArtist>();
        TPointer<R2toR::FNumericFunction> ωSpace;


        TPointer<R2toRFunctionArtist> twoPointCorrArtist = New<R2toRFunctionArtist>();
        TPointer<R2toRFunctionArtist> timeFilteredArtist = New<R2toRFunctionArtist>();


        TPointer<R2toRFunctionArtist> powerArtist        = New<R2toRFunctionArtist>();
        TPointer<R2toRFunctionArtist> amplitudesArtist   = New<R2toRFunctionArtist>();
        TPointer<R2toRFunctionArtist> phasesArtist       = New<R2toRFunctionArtist>();
        TPointer<R2toRFunctionArtist> realPartsArtist    = New<R2toRFunctionArtist>();
        TPointer<R2toRFunctionArtist> imagPartsArtist    = New<R2toRFunctionArtist>();
        TPointer<R2toC::NumericFunction> dft2DFunction;

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
        FilterSpace(const TPointer<const R2toR::FNumericFunction>& func, DevFloat tMin, DevFloat tMax)
        -> TPointer<R2toR::FNumericFunction>;

        Str GetName() const override;

        explicit FourierViewer(const TPointer<FGUIWindow>&);

        void SetFunction(Function func) override;

        void ImmediateDraw(const FPlatformWindow&) override;
    };

} // Studios::Fields

#endif //STUDIOSLAB_FOURIERVIEWER_H

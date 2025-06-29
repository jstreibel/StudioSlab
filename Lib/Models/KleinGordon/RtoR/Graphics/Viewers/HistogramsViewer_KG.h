//
// Created by joao on 7/9/24.
//

#ifndef STUDIOSLAB_HISTOGRAMSVIEWER_KG_H
#define STUDIOSLAB_HISTOGRAMSVIEWER_KG_H

#include "Graphics/DataViewers/Viewers/Viewer.h"
#include "Graphics/Plot2D/Plot2DWindow.h"

#include "KGViewer.h"

namespace Slab::Models::KGRtoR {

    using PlotWindow = Slab::Graphics::Plot2DWindow;

    class HistogramsViewer_KG : public KGViewer {
        Slab::Math::PointSet histogram_data_energy;
        Slab::Math::PointSet histogram_data_kinetic;
        Slab::Math::PointSet histogram_data_gradient;
        Slab::Math::PointSet histogram_data_potential;

        using Func = Slab::Math::R2toR::FNumericFunction;
        using FuncPointer = Slab::Pointer<Func>;

        int nbins = 200;
        bool pretty = true;
        Slab::CountType sheer_size;

        Slab::DevFloat t_min;
        Slab::DevFloat t_delta;

        Slab::Vector<Slab::Pointer<PlotWindow>> histogram_windows;

        struct HarnessData {
            using Array = Slab::Pointer<Slab::RealArray>;
            Array energy, kinetic, gradient, potential;

            explicit HarnessData(size_t sheer_size)
            : energy   (Slab::New<Slab::RealArray>(sheer_size))
            , kinetic  (Slab::New<Slab::RealArray>(sheer_size))
            , gradient (Slab::New<Slab::RealArray>(sheer_size))
            , potential(Slab::New<Slab::RealArray>(sheer_size)) {}
        };

        HarnessData harness();

    public:
        explicit HistogramsViewer_KG(const Slab::Pointer<Slab::Graphics::FGUIWindow> &);

        Str GetName() const override;

        void updateHistograms();

        void
        SetFunction(Slab::Pointer<Slab::Math::R2toR::FNumericFunction> function)
        override;

        void SetFunctionDerivative(FuncPointer pointer) override;

        void Draw() override;
    };

} // Studios::Fields::Viewers

#endif //STUDIOSLAB_HISTOGRAMSVIEWER_KG_H

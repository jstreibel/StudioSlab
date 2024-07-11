//
// Created by joao on 7/9/24.
//

#ifndef STUDIOSLAB_HISTOGRAMSVIEWER_KG_H
#define STUDIOSLAB_HISTOGRAMSVIEWER_KG_H

#include "Graphics/Viewers/Viewer.h"
#include "Graphics/Graph/PlottingWindow.h"

#include "KGViewer.h"

namespace Slab::Models::KGRtoR {

    using PlotWindow = Slab::Graphics::PlottingWindow;

    class HistogramsViewer_KG : public KGViewer {
        Slab::Math::PointSet histogram_data_energy;
        Slab::Math::PointSet histogram_data_kinetic;
        Slab::Math::PointSet histogram_data_gradient;
        Slab::Math::PointSet histogram_data_potential;

        using Func = Slab::Math::R2toR::NumericFunction;
        using FuncPointer = Slab::Pointer<Func>;

        int nbins = 200;
        bool pretty = true;
        Slab::Count sheer_size;

        Slab::Real t_min;
        Slab::Real t_delta;

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
        explicit HistogramsViewer_KG(const Slab::Pointer<Slab::Graphics::GUIWindow> &);

        void updateHistograms();

        void
        setFunction(Slab::Pointer<Slab::Math::R2toR::NumericFunction> function)
        override;

        void setFunctionDerivative(FuncPointer pointer) override;

        void draw() override;
    };

} // Studios::Fields::Viewers

#endif //STUDIOSLAB_HISTOGRAMSVIEWER_KG_H

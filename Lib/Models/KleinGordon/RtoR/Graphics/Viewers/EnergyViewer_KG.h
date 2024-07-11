//
// Created by joao on 7/10/24.
//

#ifndef STUDIOSLAB_ENERGYVIEWER_KG_H
#define STUDIOSLAB_ENERGYVIEWER_KG_H


#include "KGViewer.h"
#include "Graphics/Graph/Artists/R2toRFunctionArtist.h"
#include "Graphics/Graph/Artists/PointSetArtist.h"
#include "Graphics/Graph/PlottingWindow.h"

namespace Slab::Models::KGRtoR {

    // Grapha34f ics::Viewer
    class EnergyViewer_KG : public KGViewer {
        Graphics::PlottingWindow_ptr total_energies_window = New<Graphics::PlottingWindow>("Energy");
        Graphics::PlottingWindow_ptr temperatures_window   = New<Graphics::PlottingWindow>("Temperature");
        Graphics::PlottingWindow_ptr full_histories_window = New<Graphics::PlottingWindow>("Histories");

        Slab::Graphics::R2toRFunctionArtist_ptr f_artist{};
        Slab::Graphics::R2toRFunctionArtist_ptr dfdt_artist{};

        Slab::Graphics::R2toRFunctionArtist_ptr energy_map_artist{};
        Slab::Graphics::R2toRFunctionArtist_ptr kinetic_map_artist{};
        Slab::Graphics::R2toRFunctionArtist_ptr grad_map_artist{};
        Slab::Graphics::R2toRFunctionArtist_ptr potential_map_artist{};

        Slab::Graphics::PointSetArtist_ptr energy_history_artist{};
        Slab::Graphics::PointSetArtist_ptr kinetic_history_artist{};
        Slab::Graphics::PointSetArtist_ptr grad_history_artist{};
        Slab::Graphics::PointSetArtist_ptr potential_history_artist{};

        Pointer<PointSet> energy_history    = New<PointSet>();
        Pointer<PointSet> kinetic_history   = New<PointSet>();
        Pointer<PointSet> grad_history      = New<PointSet>();
        Pointer<PointSet> potential_history = New<PointSet>();

    public:
        explicit EnergyViewer_KG(const Pointer<Graphics::GUIWindow> &pointer);

        void setFunction(Pointer<Math::R2toR::NumericFunction> function) override;

        void setFunctionDerivative(FuncPointer pointer) override;

        void updateEnergy();
    };

} // Slab::Models::KGRtoR

#endif //STUDIOSLAB_ENERGYVIEWER_KG_H
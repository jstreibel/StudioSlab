//
// Created by joao on 7/10/24.
//

#ifndef STUDIOSLAB_ENERGYVIEWER_KG_H
#define STUDIOSLAB_ENERGYVIEWER_KG_H


#include "KGViewer.h"
#include "Graphics/Plot2D/Artists/R2toRFunctionArtist.h"
#include "Graphics/Plot2D/Artists/PointSetArtist.h"
#include "Graphics/Plot2D/Plot2DWindow.h"

namespace Slab::Models::KGRtoR {

    // Grapha34f ics::Viewer
    class EnergyViewer_KG : public KGViewer {
        Pointer<Graphics::Plot2DWindow> total_energies_window = New<Graphics::Plot2DWindow>("Energy", gui_window->GetGUIContext());
        Pointer<Graphics::Plot2DWindow> temperatures_window   = New<Graphics::Plot2DWindow>("Temperature", gui_window->GetGUIContext());
        Pointer<Graphics::Plot2DWindow> full_histories_window = New<Graphics::Plot2DWindow>("Histories", gui_window->GetGUIContext());

        Slab::Graphics::R2toRFunctionArtist_ptr f_artist{};
        Slab::Graphics::R2toRFunctionArtist_ptr dfdt_artist{};

        Slab::Graphics::R2toRFunctionArtist_ptr energy_map_artist{};
        Slab::Graphics::R2toRFunctionArtist_ptr kinetic_map_artist{};
        Slab::Graphics::R2toRFunctionArtist_ptr grad_map_artist{};
        Slab::Graphics::R2toRFunctionArtist_ptr potential_map_artist{};
        Slab::Graphics::R2toRFunctionArtist_ptr momentum_flow_map_artist{};

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

        Str getName() const override;

        void setFunction(Pointer<Math::R2toR::NumericFunction> function) override;

        void setFunctionDerivative(FuncPointer pointer) override;

        void notifyBecameVisible() override;

        void updateEnergy();
    };

} // Slab::Models::KGRtoR

#endif //STUDIOSLAB_ENERGYVIEWER_KG_H
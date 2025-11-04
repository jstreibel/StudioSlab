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
        TPointer<Graphics::FPlot2DWindow> TotalEnergiesWindow;
        TPointer<Graphics::FPlot2DWindow> TemperaturesWindow;
        TPointer<Graphics::FPlot2DWindow> FullHistoriesWindow;

        Graphics::R2toRFunctionArtist_ptr Func_Artist{};
        Graphics::R2toRFunctionArtist_ptr ddtFunc_Artist{};

        Graphics::R2toRFunctionArtist_ptr EnergyMapArtist{};
        Graphics::R2toRFunctionArtist_ptr KineticMapArtist{};
        Graphics::R2toRFunctionArtist_ptr GradMapArtist{};
        Graphics::R2toRFunctionArtist_ptr PotentialMapArtist{};
        Graphics::R2toRFunctionArtist_ptr MomentumFlowMapArtist{};

        Graphics::PointSetArtist_ptr EnergyHistoryArtist{};
        Graphics::PointSetArtist_ptr KineticHistoryArtist{};
        Graphics::PointSetArtist_ptr GradHistoryArtist{};
        Graphics::PointSetArtist_ptr PotentialHistoryArtist{};

        TPointer<FPointSet> EnergyHistory    = New<FPointSet>();
        TPointer<FPointSet> KineticHistory   = New<FPointSet>();
        TPointer<FPointSet> GradHistory      = New<FPointSet>();
        TPointer<FPointSet> PotentialHistory = New<FPointSet>();

    public:
        explicit EnergyViewer_KG(const TPointer<Graphics::FGUIWindow> &pointer);

        Str GetName() const override;

        void SetFunction(TPointer<R2toR::FNumericFunction> function) override;

        void SetFunctionDerivative(FuncPointer pointer) override;

        void NotifyBecameVisible() override;

        void UpdateEnergy();
    };

} // Slab::Models::KGRtoR

#endif //STUDIOSLAB_ENERGYVIEWER_KG_H
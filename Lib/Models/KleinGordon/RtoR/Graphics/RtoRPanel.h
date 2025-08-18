//
// Created by joao on 18/04/24.
//

#ifndef STUDIOSLAB_RTORPANEL_H
#define STUDIOSLAB_RTORPANEL_H

#include "Math/Numerics/OutputChannel.h"
#include "Math/Function/R2toR/Model/R2toRNumericFunction.h"

#include "Graphics/Window/WindowContainer/WindowPanel.h"
#include "Graphics/Window/GUIWindow.h"
#include "Graphics/Plot2D/Plot2DWindow.h"
#include "Graphics/Plot2D/Artists/R2toRFunctionArtist.h"

#include "Models/KleinGordon/RtoR/LinearStepping/KG-RtoREnergyCalculator.h"
#include "Models/KleinGordon/RtoR/LinearStepping/Output/SimHistory_Fourier.h"
#include "Models/KleinGordon/KG-NumericConfig.h"

namespace Slab::Models::KGRtoR {

    using namespace Math;
    using namespace Graphics;

    class FRtoRPanel : public WindowPanel, public FOutputChannel {
    protected:
        FGUIWindow &guiWindow;
        TPointer<FKGNumericConfig> Params;
        FKGEnergy &Hamiltonian;

        R2toR::FNumericFunction_constptr simulationHistory;
        R2toRFunctionArtist_ptr simulationHistoryArtist{};

        R2toR::FNumericFunction_constptr spaceFTHistory;
        R2toRFunctionArtist_ptr spaceFTHistoryArtist{};
        const FDFTDataHistory *dftData{};

        auto HandleOutput(const OutputPacket &packet) -> void override;

    public:
        FRtoRPanel(TPointer<FKGNumericConfig>params,
                  FGUIWindow &guiWindow,
                  FKGEnergy &hamiltonian,
                  const Str &name,
                  const Str &description);

        virtual void SetSimulationHistory(R2toR::FNumericFunction_constptr simulationHistory,
                                          const R2toRFunctionArtist_ptr &simHistoryGraph);
        virtual void SetSpaceFourierHistory(R2toR::FNumericFunction_constptr sftHistory,
                                            const FDFTDataHistory &,
                                            const R2toRFunctionArtist_ptr &sftHistoryGraph);

        virtual void notifyBecameVisible();
        virtual void notifyBecameInvisible();

    };

    DefinePointers(FRtoRPanel)

} // Graphics

#endif //STUDIOSLAB_RTORPANEL_H

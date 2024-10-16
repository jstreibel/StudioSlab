//
// Created by joao on 18/04/24.
//

#ifndef STUDIOSLAB_RTORPANEL_H
#define STUDIOSLAB_RTORPANEL_H

#include "Math/Numerics/Output/Plugs/Socket.h"
#include "Math/Function/R2toR/Model/R2toRNumericFunction.h"

#include "Graphics/Window/WindowContainer/WindowPanel.h"
#include "Graphics/Window/GUIWindow.h"
#include "Graphics/Plot2D/Plot2DWindow.h"
#include "Graphics/Plot2D/Artists/R2toRFunctionArtist.h"

#include "Models/KleinGordon/RtoR/KG-RtoREnergyCalculator.h"
#include "Models/KleinGordon/RtoR/Output/SimHistory_Fourier.h"
#include "Models/KleinGordon/KGNumericConfig.h"

namespace Slab::Models::KGRtoR {

    using namespace Math;
    using namespace Graphics;

    class RtoRPanel : public WindowPanel, public Socket {
    protected:
        GUIWindow &guiWindow;
        Pointer<KGNumericConfig> params;
        KGEnergy &hamiltonian;

        R2toR::NumericFunction_constptr simulationHistory;
        R2toRFunctionArtist_ptr simulationHistoryArtist{};

        R2toR::NumericFunction_constptr spaceFTHistory;
        R2toRFunctionArtist_ptr spaceFTHistoryArtist{};
        const DFTDataHistory *dftData{};

        auto handleOutput(const OutputPacket &packet) -> void override;

    public:
        RtoRPanel(Pointer<KGNumericConfig>params,
                  GUIWindow &guiWindow,
                  KGEnergy &hamiltonian,
                  const Str &name,
                  const Str &description);

        virtual void setSimulationHistory(R2toR::NumericFunction_constptr simulationHistory,
                                          const R2toRFunctionArtist_ptr &simHistoryGraph);
        virtual void setSpaceFourierHistory(R2toR::NumericFunction_constptr sftHistory,
                                            const DFTDataHistory &,
                                            const R2toRFunctionArtist_ptr &sftHistoryGraph);

        virtual void notifyBecameVisible();
        virtual void notifyBecameInvisible();

    };

    DefinePointers(RtoRPanel)

} // Graphics

#endif //STUDIOSLAB_RTORPANEL_H

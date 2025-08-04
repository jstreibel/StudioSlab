//
// Created by joao on 8/2/25.
//

#ifndef NUMERICALRECIPE_PLANEWAVES_H
#define NUMERICALRECIPE_PLANEWAVES_H

#include "Models/KleinGordon/RtoR/LinearStepping/KG-RtoRBuilder.h"
#include "../SquareWave.h"


namespace Modes {

    class FNumericalRecipe_PlaneWaves final : public Models::KGRtoR::KGRtoRBuilder {
        RealParameter    Q               = RealParameter   (1.0,  "Q", "Scale-invariant Q=Ak²=Aω²-4/π");
        IntegerParameter harmonic        = IntegerParameter(10, "harmonic", "Harmonic number 'n'. "
                                                                           "Wavenumber is computed as k=2πn/L. ");
        Pointer<SquareWave> SquareWave;

    public:
        FNumericalRecipe_PlaneWaves();

        auto GetBoundary() -> Base::BoundaryConditions_ptr override;

        [[nodiscard]] auto SuggestFileName() const -> Str override;
    };

} // Modes

#endif //NUMERICALRECIPE_PLANEWAVES_H

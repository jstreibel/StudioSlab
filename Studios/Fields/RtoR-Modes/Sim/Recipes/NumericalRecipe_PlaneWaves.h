//
// Created by joao on 8/2/25.
//

#ifndef NUMERICALRECIPE_PLANEWAVES_H
#define NUMERICALRECIPE_PLANEWAVES_H

#include "Models/KleinGordon/RtoR/LinearStepping/KG-RtoR-Recipe.h"
#include "../SquareWave.h"


namespace Modes {

    class FNumericalRecipePlaneWaves final : public Models::KGRtoR::FKGRtoR_Recipe
    {

        RealParameter    Q               = RealParameter   (1.0,  FParameterDescription{'Q',        "Scale-invariant Q=Ak²=Aω²-4/π"});
        IntegerParameter harmonic        = IntegerParameter(10,   FParameterDescription{"harmonic", "Harmonic number 'n'. Wavenumber is computed as k=2πn/L. "});
        TPointer<FSquareWave> p_SquareWave;

    public:
        FNumericalRecipePlaneWaves();

        auto GetBoundary() -> Base::BoundaryConditions_ptr override;

        [[nodiscard]] auto SuggestFileName() const -> Str override;
    };

    using FNumericalRecipe_PlaneWaves [[deprecated("Use FNumericalRecipePlaneWaves")]] = FNumericalRecipePlaneWaves;

} // Modes

#endif //NUMERICALRECIPE_PLANEWAVES_H

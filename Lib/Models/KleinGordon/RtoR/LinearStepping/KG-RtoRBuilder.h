//
// Created by joao on 7/30/23.
//

#ifndef STUDIOSLAB_KG_RTORBUILDER_H
#define STUDIOSLAB_KG_RTORBUILDER_H

#include "Models/KleinGordon/KG-Recipe.h"

#include "Math/Function/RtoR/Model/RtoRFunction.h"
#include "Math/Function/RtoR/Model/RtoRNumericFunction.h"

#include "KG-RtoRBoundaryCondition.h"
#include "Math/Numerics/ODE/Solver/EquationState.h"


namespace Slab::Models::KGRtoR {

    using namespace Core;

    class KGRtoRBuilder : public Slab::Models::KGRecipe {
    protected:
        IntegerParameter Potential = IntegerParameter(2, "V,potential", "Potential of wave equation:"
                                                                        "\n\t 0: massless"
                                                                        "\n\t 1: Klein-Gordon"
                                                                        "\n\t 2: signum-Gordon"
                                                                        "\n\t 3: regular SG expansion");
        RealParameter massSqr  = RealParameter(1.0, "M,massSqr",   "Squared mass of the Klein-Gordon potential (on-shell ω²-k²-m²=0), if chosen.");
        IntegerParameter N_num = IntegerParameter(15, "O,N_num", "Order of regular SG expansion.");
        IntegerParameter BoundaryConditions = IntegerParameter(0, "b,boundary_condition", "Boundary space conditions (affects Laplacian): "
                                                                        "\n\t 0: fixed"
                                                                        "\n\t 1: periodic");

        bool force_periodicBC = false;          // Gambiarris (ish)

        Vector<TPointer<Socket>> getTimeDFTSnapshots();
        auto _newTimeDFTSnapshotOutput(const Str& folder, DevFloat t_start, DevFloat t_end, const FRealVector &x_locations) const -> TPointer<Socket>;

        virtual auto buildOpenGLOutput() -> void*;

        void setLaplacianPeriodicBC();    // Gambiarris
        void setLaplacianFixedBC();       // Gambiarris
        bool usesPeriodicBC() const;      // Gambiarris

    public:
        auto suggestFileName() const -> Str override;

    public:
        explicit KGRtoRBuilder(const Str& name,
                           const Str& generalDescription,
                           bool doRegister=false);

        virtual ~KGRtoRBuilder() = default;

        RtoR::Function_ptr getPotential() const;
        virtual TPointer<Base::FunctionT<DevFloat, DevFloat>> getNonHomogenous();

        auto buildOutputSockets()   -> Vector<TPointer<Socket>> override;

        auto buildSolver()  -> TPointer<Base::LinearStepSolver> override;

        void *getHamiltonian() override;

        auto getInitialState() const      -> KGRtoR::EquationState_ptr;
        virtual auto getBoundary()  -> Base::BoundaryConditions_ptr = 0;
        auto newFunctionArbitrary() const -> Math::RtoR::NumericFunction_ptr;
        auto newFieldState() const        -> KGRtoR::EquationState_ptr;

    };

    DefinePointers(KGRtoRBuilder)
}

#endif //STUDIOSLAB_KG_RTORBUILDER_H

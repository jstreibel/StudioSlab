//
// Created by joao on 7/30/23.
//

#ifndef STUDIOSLAB_KG_RTORBUILDER_H
#define STUDIOSLAB_KG_RTORBUILDER_H

#include "Models/KleinGordon/KG-Recipe.h"

#include "Math/Function/RtoR/Model/RtoRFunction.h"
#include "Math/Function/RtoR/Model/RtoRNumericFunction.h"

#include "KG-RtoRBoundaryCondition.h"


namespace Slab::Models::KGRtoR {

    using namespace Core;

    class FKGRtoRPotentialOptions : public FInterfaceOwner
    {
    public:
        IntegerParameter Potential = IntegerParameter(2, FParameterDescription{'V', "Potential",                      "Potential of wave equation:"
                                                                                                                      "\n\t 0: massless"
                                                                                                                      "\n\t 1: Klein-Gordon"
                                                                                                                      "\n\t 2: signum-Gordon"
                                                                                                                      "\n\t 3: regular SG expansion", FLongOptionFormatting{false}});
        RealParameter    MassSquared =        RealParameter   (1.0, FParameterDescription{'M', "Mass Squared",        "Squared mass of the Klein-Gordon potential (on-shell ω²-k²-m²=0), if chosen.", FLongOptionFormatting{true}});
        IntegerParameter N_num =              IntegerParameter(15,  FParameterDescription{'O', "expansion N",         "Order of regular SG expansion."});

        explicit FKGRtoRPotentialOptions(bool bDoRegister);
    };

    class FKGRtoR_Recipe : public KGRecipe {
    protected:
        IntegerParameter BoundaryConditions = IntegerParameter(0,   FParameterDescription{'b', "Boundary Conditions", "Boundary space conditions (affects Laplacian): "
                                                                                                                      "\n\t 0: fixed"
                                                                                                                      "\n\t 1: periodic",
                                                                                                                      FLongOptionFormatting{true}});

        bool force_periodicBC = false;          // Gambiarris (ish)

        FKGRtoRPotentialOptions PotentialOptions;

        Vector<TPointer<FOutputChannel>> getTimeDFTSnapshots();
        auto _newTimeDFTSnapshotOutput(const Str& folder, DevFloat t_start, DevFloat t_end, const FRealVector &x_locations) const -> TPointer<FOutputChannel>;

        virtual auto BuildOpenGLOutput() -> void*;

        void SetLaplacianPeriodicBC();    // Gambiarris
        void SetLaplacianFixedBC();       // Gambiarris
        [[nodiscard]] bool usesPeriodicBC() const;      // Gambiarris

    public:
        [[nodiscard]] auto SuggestFileName() const -> Str override;

        explicit FKGRtoR_Recipe(const Str& name,
                           const Str& generalDescription,
                           bool doRegister=false);

        ~FKGRtoR_Recipe() override = default;

        [[nodiscard]] RtoR::Function_ptr getPotential() const;
        virtual TPointer<Base::FunctionT<DevFloat, DevFloat>> GetNonHomogenousTerm();

        auto BuildOutputSockets()   -> Vector<TPointer<FOutputChannel>> override;

        auto buildSolver()  -> TPointer<Base::LinearStepSolver> override;

        void *getHamiltonian() override;

        [[nodiscard]] auto getInitialState() const      -> KGRtoR::EquationState_ptr;
        virtual auto GetBoundary()  -> Base::BoundaryConditions_ptr = 0;
        [[nodiscard]] auto newFunctionArbitrary() const -> Math::RtoR::NumericFunction_ptr;
        [[nodiscard]] auto NewFieldState() const        -> KGRtoR::EquationState_ptr;

    };

    DefinePointers(FKGRtoR_Recipe)
}

#endif //STUDIOSLAB_KG_RTORBUILDER_H

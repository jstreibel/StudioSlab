//
// Created by joao on 7/10/24.
//

#ifndef STUDIOSLAB_KGVIEWER_H
#define STUDIOSLAB_KGVIEWER_H

#include "Graphics/Viewers/Viewer.h"
#include "Models/KleinGordon/RtoR/KG-RtoREnergyCalculator.h"
#include "Math/Function/RtoR/Model/FunctionsCollection/AbsFunction.h"

#include "Utils/Pointer.h"

namespace Slab::Models::KGRtoR {
    using FuncPointer = Pointer<Math::R2toR::NumericFunction>;
    using ConstFuncPointer = Pointer<const Math::R2toR::NumericFunction>;

    class KGViewer : public Graphics::Viewer {

        FuncPointer ddt_base_function;

    public:
        explicit KGViewer(const Pointer<Graphics::GUIWindow> &
               , const Pointer<Math::R2toR::NumericFunction> &func = nullptr
               , const Pointer<Math::R2toR::NumericFunction> &ddt_func = nullptr);

        virtual void
        setFunctionDerivative(FuncPointer);

        auto
        getFunctionDerivative() const
        -> ConstFuncPointer;

        static KGEnergy getHamiltonian();

        bool areFunctionsConsistent() const;


    };

} // Slab::Models::KGRtoR

#endif //STUDIOSLAB_KGVIEWER_H

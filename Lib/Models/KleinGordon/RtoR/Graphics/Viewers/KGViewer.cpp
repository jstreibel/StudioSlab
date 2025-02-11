//
// Created by joao on 7/10/24.
//

#include "KGViewer.h"

#include <utility>

namespace Slab::Models::KGRtoR {

    KGViewer::KGViewer(const Pointer<Graphics::GUIWindow> &gui_window,
                       const Pointer<Math::R2toR::NumericFunction> &func,
                       const Pointer<Math::R2toR::NumericFunction> &ddt_func)
    : Viewer(gui_window, func), ddt_base_function(ddt_func) {}

    void KGViewer::setFunctionDerivative(FuncPointer derivs) {
        ddt_base_function = std::move(derivs);
    }

    auto KGViewer::getFunctionDerivative() const -> ConstFuncPointer {
        return ddt_base_function;
    }

    KGEnergy KGViewer::getHamiltonian() {
        static Math::RtoR::AbsFunction abs;
        static KGEnergy hamiltonian(Naked(abs));

        return hamiltonian;
    }

    bool KGViewer::areFunctionsConsistent() const {
        auto a = getFunction();
        auto b = getFunctionDerivative();

        if(a==nullptr || b==nullptr) return false;

        for(int dim=0; dim<2; ++dim)
            if(a->getSpace().getMetaData().getN(dim) != b->getSpace().getMetaData().getN(dim))
                return false;

        return a->getDomain()==b->getDomain();
    }


} // Slab::Models::KGRtoR
//
// Created by joao on 7/10/24.
//

#include "KGMainViewer.h"

namespace Slab::Models::KGRtoR {
    void KGMainViewer::addKGViewer(const Pointer<KGViewer> &kg_viewer) {
        kg_viewers.emplace_back(kg_viewer);
        addViewer(kg_viewer);
    }

    bool KGMainViewer::setCurrentViewer(Slab::Index i) {
        auto value = MainViewer::setCurrentViewer(i);

        auto kg_viewer = getCurrentKGViewer();
        if(kg_viewer){
            ddtbase_function = getFunctionTimeDerivative();
            kg_viewer->setFunctionDerivative(ddtbase_function);
        }

        return value;
    }

    void KGMainViewer::setFunctionTimeDerivative(Pointer<R2toR::NumericFunction> func) {
        ddtbase_function = std::move(func);

        for(auto &kg_viewer : kg_viewers) kg_viewer->setFunctionDerivative(ddtbase_function);
    }

    Pointer<R2toR::NumericFunction> KGMainViewer::getFunctionTimeDerivative() {
        return ddtbase_function;
    }

    Slab::Pointer<KGViewer> KGMainViewer::getCurrentKGViewer() {
        auto curr_viewer = getCurrentViewer();

        if(!Slab::Contains(kg_viewers, curr_viewer))
            return nullptr;

        return Slab::DynamicPointerCast<Slab::Models::KGRtoR::KGViewer>(curr_viewer);
    }

} // Slab::Models::KGRtoR
//
// Created by joao on 7/10/24.
//

#include "KGMainViewer.h"

namespace Slab::Models::KGRtoR {
    void FKGMainViewer::addKGViewer(const TPointer<FKGViewer> &kg_viewer) {
        kg_viewers.emplace_back(kg_viewer);
        addViewer(kg_viewer);
    }

    bool FKGMainViewer::setCurrentViewer(Slab::Index i) {
        auto value = FMainViewer::setCurrentViewer(i);

        auto kg_viewer = getCurrentKGViewer();
        if(kg_viewer){
            ddtbase_function = getFunctionTimeDerivative();
            kg_viewer->SetFunctionDerivative(ddtbase_function);
        }

        return value;
    }

    void FKGMainViewer::setFunctionTimeDerivative(TPointer<R2toR::FNumericFunction> func) {
        ddtbase_function = std::move(func);

        for(auto &kg_viewer : kg_viewers) kg_viewer->SetFunctionDerivative(ddtbase_function);
    }

    TPointer<R2toR::FNumericFunction> FKGMainViewer::getFunctionTimeDerivative() {
        return ddtbase_function;
    }

    Slab::TPointer<FKGViewer> FKGMainViewer::getCurrentKGViewer() {
        auto curr_viewer = getCurrentViewer();

        if(!Slab::Contains(kg_viewers, curr_viewer))
            return nullptr;

        return Slab::DynamicPointerCast<Slab::Models::KGRtoR::FKGViewer>(curr_viewer);
    }

} // Slab::Models::KGRtoR
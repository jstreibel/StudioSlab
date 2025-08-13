#ifndef KG_DISPERSION_RELATION_H
#define KG_DISPERSION_RELATION_H

//
// Created by joao on 24/09/23.
//

#include "Math/Function/RtoR/Model/RtoRFunction.h"

namespace Slab::Math::RtoR {

    enum KGDispersionRelationMode {
        ω_AsFunctionOf_k,
        k_AsFunctionOf_ω
    };

    class KGDispersionRelation : public RtoR::Function {
        DevFloat m²;
    public:
        KGDispersionRelationMode mode;

        enum Branch {
            Positive,
            Negative
        } branch;

        KGDispersionRelation(DevFloat mass, KGDispersionRelationMode, Branch branch=Positive);

        DevFloat operator()(DevFloat x) const override;

        auto domainContainsPoint(DevFloat x) const -> bool override;
    };

    class KGDispersionRelation_high_k : public RtoR::Function {
        DevFloat m²;
    public:
        KGDispersionRelationMode mode;

        explicit KGDispersionRelation_high_k(DevFloat mass, KGDispersionRelationMode);

        DevFloat operator()(DevFloat k) const override;

        auto domainContainsPoint(DevFloat k) const -> bool override;
    };

}

#endif
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
        Real m²;
    public:
        KGDispersionRelationMode mode;

        enum Branch {
            Positive,
            Negative
        } branch;

        KGDispersionRelation(Real mass, KGDispersionRelationMode, Branch branch=Positive);

        Real operator()(Real x) const override;

        auto domainContainsPoint(Real x) const -> bool override;
    };

    class KGDispersionRelation_high_k : public RtoR::Function {
        Real m²;
    public:
        KGDispersionRelationMode mode;

        explicit KGDispersionRelation_high_k(Real mass, KGDispersionRelationMode);

        Real operator()(Real k) const override;

        auto domainContainsPoint(Real k) const -> bool override;
    };

}

#endif
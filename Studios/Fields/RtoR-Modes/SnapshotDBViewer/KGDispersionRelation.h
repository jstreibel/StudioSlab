#ifndef KG_DISPERSION_RELATION_H
#define KG_DISPERSION_RELATION_H

//
// Created by joao on 24/09/23.
//

#include "Math/Function/RtoR/Model/RtoRFunction.h"

namespace Slab::Math::RtoR {

    enum EKGDispersionRelationMode {
        ω_AsFunctionOf_k,
        k_AsFunctionOf_ω
    };

    using KGDispersionRelationMode [[deprecated("Use EKGDispersionRelationMode")]] = EKGDispersionRelationMode;

    class FKGDispersionRelation : public RtoR::Function {
        DevFloat m²;
    public:
        EKGDispersionRelationMode mode;

        enum Branch {
            Positive,
            Negative
        } branch;

        FKGDispersionRelation(DevFloat mass, EKGDispersionRelationMode, Branch branch=Positive);

        DevFloat operator()(DevFloat x) const override;

        auto domainContainsPoint(DevFloat x) const -> bool override;
    };

    class FKGDispersionRelationHighK : public RtoR::Function {
        DevFloat m²;
    public:
        EKGDispersionRelationMode mode;

        explicit FKGDispersionRelationHighK(DevFloat mass, EKGDispersionRelationMode);

        DevFloat operator()(DevFloat k) const override;

        auto domainContainsPoint(DevFloat k) const -> bool override;
    };

    using KGDispersionRelation [[deprecated("Use FKGDispersionRelation")]] = FKGDispersionRelation;
    using KGDispersionRelation_high_k [[deprecated("Use FKGDispersionRelationHighK")]] = FKGDispersionRelationHighK;

}

#endif

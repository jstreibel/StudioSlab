#ifndef KG_DISPERSION_RELATION_H
#define KG_DISPERSION_RELATION_H

//
// Created by joao on 24/09/23.
//

#include "Math/Function/Maps/RtoR/Model/RtoRFunction.h"

namespace RtoR {

    class KGDispersionRelation : public RtoR::Function {
        Real m²;
    public:
        enum KGDRMode {
            ω_AsFunctionOf_k,
            k_AsFunctionOf_ω
        } mode;

        enum Branch {
            Positive,
            Negative
        } branch;

        KGDispersionRelation(Real mass, KGDRMode, Branch branch=Positive);

        Real operator()(Real x) const override;

        auto domainContainsPoint(Real x) const -> bool override;
    };

    class KGDispersionRelation_high_k : public RtoR::Function {
        Real m²;
    public:
        enum KGDRMode {
            ω_AsFunctionOf_k,
            k_AsFunctionOf_ω
        } mode;

        explicit KGDispersionRelation_high_k(Real mass, KGDRMode);

        Real operator()(Real k) const override;

        auto domainContainsPoint(Real k) const -> bool override;
    };

}

#endif
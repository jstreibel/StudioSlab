#ifndef KG_DISPERSION_RELATION_H
#define KG_DISPERSION_RELATION_H

//
// Created by joao on 24/09/23.
//

#include "Maps/RtoR/Model/RtoRFunction.h"

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

}

#endif
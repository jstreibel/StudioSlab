//
// Created by joao on 24/09/23.
//

#include "KGDispersionRelation.h"

#define k² (x*x)
#define ω² (x*x)

namespace Slab::Math::RtoR {

    FKGDispersionRelation::FKGDispersionRelation(DevFloat mass,
                                               EKGDispersionRelationMode mode,
                                               FKGDispersionRelation::Branch branch)
    : m²(mass*mass), mode (mode), branch(branch) {}

    DevFloat FKGDispersionRelation::operator()(DevFloat x) const {
        fix sign = branch == Positive ? 1. : -1.;

        switch (mode) {
            case ω_AsFunctionOf_k:
                return sign * sqrt(k² +m²);
            case k_AsFunctionOf_ω:
                return sign * sqrt(ω² -m²);
        }

        NOT_IMPLEMENTED
    }

    bool FKGDispersionRelation::domainContainsPoint(DevFloat x) const {
        switch (mode) {
            case ω_AsFunctionOf_k:
                return true;
            case k_AsFunctionOf_ω:
                return ω² > m²;
        }

        NOT_IMPLEMENTED
    }

    FKGDispersionRelationHighK::FKGDispersionRelationHighK(DevFloat mass, EKGDispersionRelationMode mode)
    : m²(mass*mass), mode(mode) {   }

    DevFloat FKGDispersionRelationHighK::operator()(DevFloat x) const {
        const auto k = x;
        const auto ω = x;
        switch (mode) {
            case ω_AsFunctionOf_k:
                return k + 0.5 * m²/k - 0.125 * m²*m²/(k *k *k);
            case k_AsFunctionOf_ω:
                return ω - 0.5 * m²/ω + 0.125 * m²*m²/(ω *ω *ω);
        }

        NOT_IMPLEMENTED
    }

    auto FKGDispersionRelationHighK::domainContainsPoint(DevFloat x) const -> bool {
        return x != 0.0;
    }


}
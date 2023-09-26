//
// Created by joao on 24/09/23.
//

#include "KGDispersionRelation.h"

#define k² (x*x)
#define ω² (x*x)

RtoR::KGDispersionRelation::KGDispersionRelation(Real mass, KGDRMode mode, Branch branch)
: m²(mass*mass), mode(mode), branch(branch) { }

Real RtoR::KGDispersionRelation::operator()(Real x) const {
    fix sign = branch==Positive?1.:-1.;

    switch (mode) {
        case ω_AsFunctionOf_k: return sign*sqrt(k² + m²);
        case k_AsFunctionOf_ω: return sign*sqrt(ω² - m²);
    }

    NOT_IMPLEMENTED
}

bool RtoR::KGDispersionRelation::domainContainsPoint(Real x) const {
    switch (mode) {
        case ω_AsFunctionOf_k: return true;
        case k_AsFunctionOf_ω: return ω² > m²;
    }

    NOT_IMPLEMENTED
}


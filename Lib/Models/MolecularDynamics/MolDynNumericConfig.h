//
// Created by joao on 10/16/24.
//

#ifndef STUDIOSLAB_MOLDYNNUMERICCONFIG_H
#define STUDIOSLAB_MOLDYNNUMERICCONFIG_H

#include "Math/Numerics/SimConfig/NumericConfig.h"

namespace Slab::Models::MolecularDynamics {

    class MolDynNumericConfig : public Math::NumericConfig {
    public:
        auto getn() const -> UInt override;

        auto getN() const -> Count;

        auto getL() const -> Real;

        auto gett() const -> Real;

        auto sett(Real) -> void;

        auto getdt() const -> Real;

        auto getr() const -> Real;

        auto to_string() const -> Str override;
    };

} // Slab::Models::MolecularDynamics

#endif //STUDIOSLAB_MOLDYNNUMERICCONFIG_H

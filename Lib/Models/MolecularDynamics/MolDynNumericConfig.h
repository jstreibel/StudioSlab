//
// Created by joao on 10/16/24.
//

#ifndef STUDIOSLAB_MOLDYNNUMERICCONFIG_H
#define STUDIOSLAB_MOLDYNNUMERICCONFIG_H

#include "Models/DynamicsNumericConfig.h"

namespace Slab::Models::MolecularDynamics {

    class MolDynNumericConfig : public DynamicsNumericConfig {
        IntegerParameter::Ptr n = IntegerParameter::New(10000, "steps", "Total timesteps to simulate.");

    public:
        MolDynNumericConfig(bool do_register=true);

        auto getn() const -> UInt override;

        auto getdt() const -> DevFloat;

        auto to_string() const -> Str override;
    };

} // Slab::Models::MolecularDynamics

#endif //STUDIOSLAB_MOLDYNNUMERICCONFIG_H

//
// Created by joao on 10/16/24.
//

#include "DynamicsNumericConfig.h"
#include "Core/Tools/Log.h"
#include "Utils/List.h"

namespace Slab::Models {

    DynamicsNumericConfig::DynamicsNumericConfig(bool do_register)
    : Math::NumericConfig(false)
    {
        interface->addParameters({N, L, t});

        if(do_register) registerToManager();
    }

    auto DynamicsNumericConfig::getN() const -> UInt { return **N; }

    auto DynamicsNumericConfig::getL() const -> floatt { return **L; }

    auto DynamicsNumericConfig::gett() const -> floatt { return **t; }

    auto DynamicsNumericConfig::sett(Real t_max) -> void {
        Log::Attention() << "Command line argument '" << t->getCommandLineArgumentName(true) << "' "
                         << "being overridden and set to " << t_max << ";" << Log::Flush;

        t->setValue(t_max);
    }

    void DynamicsNumericConfig::notifyCLArgsSetupFinished() {
        Math::NumericConfig::notifyCLArgsSetupFinished();

        if (**t < 0) {
            *t = **L * .5;
            Log::Attention("NumericParams") << " parameter 't' is <0. Setting to t = L/2 = " << *t;
        }
    }


} // Models
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
        Interface->AddParameters({N, L, t});

        if(do_register) RegisterToManager();
    }

    auto DynamicsNumericConfig::getN() const -> UInt { return **N; }

    auto DynamicsNumericConfig::getL() const -> floatt { return **L; }

    auto DynamicsNumericConfig::gett() const -> floatt { return **t; }

    auto DynamicsNumericConfig::sett(DevFloat t_max) -> void {
        Log::Attention() << "Command line argument '" << t->getCommandLineArgumentName(true) << "' "
                         << "being overridden and set to " << t_max << ";" << Log::Flush;

        t->SetValue(t_max);
    }

    void DynamicsNumericConfig::NotifyCLArgsSetupFinished() {
        Math::NumericConfig::NotifyCLArgsSetupFinished();

        if (**t < 0) {
            *t = **L * .5;
            Log::Attention("NumericParams") << " parameter 't' is <0. Setting to t = L/2 = " << *t;
        }
    }


} // Models
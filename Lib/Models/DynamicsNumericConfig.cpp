//
// Created by joao on 10/16/24.
//

#include "DynamicsNumericConfig.h"
#include "Core/Tools/Log.h"
#include "Utils/List.h"

namespace Slab::Models {

    DynamicsNumericConfig::DynamicsNumericConfig(bool do_register)
    : Math::FNumericConfig(false)
    {
        Interface->AddParameters({N, L, t});

        if(do_register) RegisterToManager();
    }

    auto DynamicsNumericConfig::getN() const -> UInt { return **N; }

    auto DynamicsNumericConfig::GetL() const -> floatt { return **L; }

    auto DynamicsNumericConfig::Get_t() const -> floatt
    {
        if (**t < 0) {
            *t = **L * .5;
            Log::Attention("NumericParams") << " parameter 't' is <0. Setting to t = L/2 = " << *t;
        }
        return **t;
    }

    auto DynamicsNumericConfig::sett(DevFloat t_max) -> void {
        Log::Attention() << "Command line argument '" << t->GetCommandLineArgumentName(true) << "' "
                         << "being overridden and set to " << t_max << ";" << Log::Flush;

        t->SetValue(t_max);
    }

    void DynamicsNumericConfig::NotifyInterfaceSetupIsFinished() {
        Math::FNumericConfig::NotifyInterfaceSetupIsFinished();

        (void)Get_t();
    }


} // Models
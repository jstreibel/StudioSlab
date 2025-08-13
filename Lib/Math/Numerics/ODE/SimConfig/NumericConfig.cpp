
#include "Core/Controller/CommandLine/CommandLineInterfaceManager.h"
#include "NumericConfig.h"


namespace Slab::Math {
    NumericConfig::NumericConfig(bool do_register)
    : Core::FCommandLineInterfaceOwner("Numeric Parameters,The core parameters that define the simulation per-se",
                             0,
                             do_register)
    {
    }
}
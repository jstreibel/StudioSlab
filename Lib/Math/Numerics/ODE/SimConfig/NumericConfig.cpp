
#include "../../../../Core/Controller/InterfaceManager.h"
#include "NumericConfig.h"


namespace Slab::Math {
    FNumericConfig::FNumericConfig(bool do_register)
    : Core::FInterfaceOwner("Numeric Parameters,The core parameters that define the simulation per-se",
                             0,
                             do_register)
    {
    }
}
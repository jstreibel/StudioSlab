
#include "Core/Controller/CommandLine/CLInterfaceManager.h"
#include "NumericConfig.h"


namespace Slab::Math {
    NumericConfig::NumericConfig(bool doRegister, std::initializer_list<Pointer<Core::CLParameter>> parameters)
    : Core::CLInterfaceOwner("Numeric Parameters,The core parameters that define the simulation per-se", 0, doRegister) {
        interface->addParameters(parameters);
    }
}
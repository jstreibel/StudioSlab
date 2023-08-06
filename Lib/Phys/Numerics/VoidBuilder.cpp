//
// Created by joao on 10/17/21.
//

#include "Base/Controller/Interface/InterfaceSelector.h"
#include "Base/Controller/Interface/InterfaceManager.h"
#include "Base/Tools/Log.h"
#include "VoidBuilder.h"
//#include "Phys/Numerics/Method/Method-RK4.h"

#define GENERATE_FOR_NTHREADS(STEPPER_TYPE, N) \
case (N): \
stepper = new STEPPER_TYPE<N, EqStateType>(dPhi); \
break;

#define DONT_REGISTER false

Base::Simulation::VoidBuilder::VoidBuilder(Str name, Str generalDescription)
: InterfaceOwner(name, 100, DONT_REGISTER)
, numericParams(DONT_REGISTER)
, dev(DONT_REGISTER)
, prefix(name)
{
    interface->addParameters({&noHistoryToFile, &outputResolution,
                              &VisualMonitor, &VisualMonitor_startPaused, &OpenGLMonitor_stepsPerIdleCall
                                     /*&takeSnapshot, &snapshotTime, */ });

    interface->addSubInterface(numericParams.getInterface());
    interface->addSubInterface(dev.getInterface());

    Log::Info() << "SimulationBuilder '" << interface->getName() << "': \""
                << interface->getGeneralDescription() << "\" instantiated." << Log::Flush;
}

auto Base::Simulation::VoidBuilder::getNumericParams() const -> const NumericParams & {
    return numericParams;
}

auto Base::Simulation::VoidBuilder::getDevice() const -> const Device & {
    return dev;
}

Str Base::Simulation::VoidBuilder::suggestFileName() const {
    const auto SEPARATOR = " ";
    auto strParams = numericParams.getInterface()->toString({"L", "N"}, SEPARATOR);

    auto str = prefix + SEPARATOR + strParams;

    return str;
}

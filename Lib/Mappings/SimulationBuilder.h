#ifndef SIMULATION_H
#define SIMULATION_H

#include "Phys/Numerics/Output/Builder.h"
#include "Base/Controller/Interface/Interface.h"

namespace Base {

    class SimulationBuilder : public InterfaceOwner {
    protected:
        Numerics::OutputSystem::Builder::Ptr outputSystemBuilder;
        String prefix = "";

        explicit SimulationBuilder(String generalDescription, Numerics::OutputSystem::Builder::Ptr osb,
                                   String prefix="");

    public:
        typedef std::shared_ptr<SimulationBuilder> Ptr;

        virtual auto getBoundary()        const -> const void * = 0;
        virtual auto buildOutputManager()       -> OutputManager * = 0;
        virtual auto registerAllocator()  const -> void;;

        auto toString()                   const -> String;
    };

}


#endif // SIMULATION_H

#ifndef SIMULATION_H
#define SIMULATION_H

#include "Phys/Numerics/Output/StructureBuilder.h"
#include "Base/Controller/Interface/Interface.h"

namespace Base {

    class SimulationBuilder : public Interface {
    protected:
        Numerics::OutputSystem::StructureBuilder *outputStructureBuilder;
        String prefix = "";

        explicit SimulationBuilder(String generalDescription, Numerics::OutputSystem::StructureBuilder* osb, String prefix="", bool selfRegister=false);
    public:
        typedef std::shared_ptr<SimulationBuilder> Ptr;

        virtual auto getBoundary() const -> const void * = 0;

        virtual auto buildOutputManager() -> OutputManager * = 0;

        auto getOutputStructureBuilder() -> Numerics::OutputSystem::StructureBuilder *;

        auto toString() const -> String override;
    };

}


#endif // SIMULATION_H

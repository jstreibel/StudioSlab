#ifndef SIMULATION_H
#define SIMULATION_H

#include <Phys/Numerics/Output/OutputStructureBuilderBase.h>
#include "Studios/Controller/Interface/Interface.h"

namespace Base {

    /** Boundary conditions interface
     *
     */
    class BCInterface : public Interface {
    protected:
        OutputStructureBuilderBase *outputStructureBuilder;
        String prefix = "";
    public:
        explicit BCInterface(String generalDescription, OutputStructureBuilderBase* osb, String prefix="", bool selfRegister=false);

        virtual auto getBoundary() const -> const void * = 0;

        virtual auto buildOutputManager() -> OutputManager * = 0;

        auto getOutputStructureBuilder() -> Interface *;

        auto toString() const -> String override;
    };

}


#endif // SIMULATION_H

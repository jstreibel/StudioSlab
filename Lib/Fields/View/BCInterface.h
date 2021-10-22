#ifndef SIMULATION_H
#define SIMULATION_H

#include "OutputStructureBuilderBase.h"
#include "Studios/Controller/Interface/Interface.h"

namespace Base {

    /** Boundary conditions interface
     *
     */
    class BCInterface : public Interface {
    protected:
        OutputStructureBuilderBase *outputStructureBuilder;
    public:
        explicit BCInterface(String generalDescription, OutputStructureBuilderBase* osb, bool selfRegister=false);

        virtual auto getBoundary() const -> const void * = 0;

        virtual auto buildOutputManager() -> OutputManager * = 0;

        auto getOutputStructureBuilder() -> Interface *;
    };

}


#endif // SIMULATION_H

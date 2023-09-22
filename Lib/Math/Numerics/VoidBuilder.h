#ifndef SIMULATION_H
#define SIMULATION_H

#include "Math/Numerics/Method/Stepper.h"
#include "Math/Numerics/Output/OutputManager.h"

#include "Core/Controller/Interface/Interface.h"
#include "Math/Numerics/SimConfig/SimulationConfig.h"

namespace Core::Simulation {

    class VoidBuilder : public InterfaceOwner {
    protected:
        SimulationConfig simulationConfig;

        Str prefix;

        explicit VoidBuilder(const Str& name, Str generalDescription, bool doRegister = false);

    public:
        typedef std::shared_ptr<VoidBuilder> Ptr;

        VoidBuilder() = delete;
        virtual ~VoidBuilder() = default;

        virtual auto buildOutputManager()         -> OutputManager * = 0;
        virtual auto buildEquationSolver()        -> void * = 0;
        virtual auto buildStepper()               -> Stepper* = 0;

        virtual auto suggestFileName()      const -> Str;

        virtual auto getBoundary()                -> void * = 0;
        virtual auto getInitialState()            -> void * = 0;


        virtual
        auto newFunctionArbitrary()       -> void * = 0;
        template<class ARB_FUNC_TYPE>
        auto NewFunctionArbitrary()       -> ARB_FUNC_TYPE *;

        virtual
        auto newFieldState()              -> void * = 0;
        template<typename StateType>
        auto NewFieldState()              -> StateType*;

        auto getNumericParams()             const -> const NumericConfig &;
        auto getDevice()                    const -> const DeviceConfig &;
    };

    template<typename StateType>
    auto VoidBuilder::NewFieldState() -> StateType * {
        return (StateType*)this->newFieldState();
    }

    template<class ARB_FUNC_TYPE>
    ARB_FUNC_TYPE *VoidBuilder::NewFunctionArbitrary() {
        return (ARB_FUNC_TYPE *) newFunctionArbitrary();
    }
}



#endif // SIMULATION_H

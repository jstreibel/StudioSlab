//
// Created by joao on 1/09/23.
//

#include "Builder.h"

#include "Math/Constants.h"

#include "Models/KleinGordon/RtoR/KG-RtoRBoundaryCondition.h"
#include "Mappings/RtoR/Model/FunctionsCollection/NullFunction.h"

#include "Core/Tools/Log.h"
#include "Core/Controller/Interface/InterfaceManager.h"
#include "Monitor.h"

#define DONT_REGISTER false

namespace Modes {

    class BC : public RtoR::BoundaryCondition {
    public:
        Real A, ω;

        BC(const RtoR::EquationState &prototype, Real A, Real ω)
        : BoundaryCondition(prototype, new RtoR::NullFunction(), new RtoR::NullFunction())
        , A(A)
        , ω(ω) {}

        void apply(EqState &toFunction, Real t) const override {
            if(t==0.0) RtoR::BoundaryCondition::apply(toFunction, t);

            OUT ϕ = toFunction.getPhi();
            OUT 𝜕ₜϕ = toFunction.getDPhiDt() ;

            ϕ.getSpace().getHostData()[0] = A*sin(ω*t);
            𝜕ₜϕ.getSpace().getHostData()[0] = A*ω*cos(ω*t);
        }
    };

    Builder::Builder(bool doRegister)
    : RtoR::KGBuilder("Modes", "Test SG response to different modes and amplitudes of harmonic oscillation", DONT_REGISTER)
    {
        interface->addParameters({&A, &omega});

        if(doRegister) InterfaceManager::getInstance().registerInterface(interface);
    }

    void *Builder::getBoundary() {
        auto &prototype = *(RtoR::EquationState*) RtoR::KGBuilder::newFieldState();
        return new Modes::BC(prototype, *A, *omega);
    }

    void Builder::notifyCLArgsSetupFinished() {
        InterfaceOwner::notifyCLArgsSetupFinished();
        fix period = 2*Constants::pi / *omega;
        fix L = simulationConfig.numericConfig.getL();
        fix n = simulationConfig.numericConfig.getn();
        fix res = (period/L)*n;

        Log::Info() << Log::BGWhite+Log::FGBlack << " Technical sine resolution is " << res << " samples/cycle " << Log::ResetFormatting << Log::Flush;
    }

    RtoR::Monitor *Builder::buildOpenGLOutput() {
        fix amp = (*A) * 1.1;
        auto monitor = new Modes::Monitor(simulationConfig.numericConfig, *(RtoR::KGEnergy*)getHamiltonian(), -amp, +amp, "Modes monitor");

        monitor->setInputModes({*A}, {*omega});

        return monitor;
    }

    void *Builder::buildEquationSolver() {
        return KGBuilder::buildEquationSolver();
    }

} // Modes
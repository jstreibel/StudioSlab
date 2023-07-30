//
// Created by joao on 4/1/23.
//

#ifndef STUDIOSLAB_SIGNAL_H
#define STUDIOSLAB_SIGNAL_H


#include <jack/types.h>
#include "Mappings/RtoR/Controller/RtoRBCInterface.h"

#include "Mappings/RtoR/Model/RtoRFieldState.h"

#include "Mappings/RtoR/Model/FunctionsCollection/AnalyticOscillon.h"
#include "Mappings/RtoR/Model/FunctionsCollection/NullFunction.h"

#include "Phys/DifferentialEquations/BoundaryConditions.h"
#include "Phys/Numerics/Output/Plugs/Socket.h"


const auto pi = 3.1415926535897932384626;

namespace RtoR {

    namespace Signal {
        extern Real xInitDampCutoff_normalized;
        extern Real dampFactor;


        class JackOutput : public Numerics::OutputSystem::Socket {
            auto _out(const OutputPacket &packet) -> void override;

        public:
            JackOutput(const NumericParams &params);
            auto shouldOutput(Real t, unsigned long timestep) -> bool override;

        };


        class OutputBuilder : public OutputStructureBuilderRtoR {
        protected:
        public:
            auto build(Str outputFileName) -> OutputManager * override;

        protected:
            auto buildOpenGLOutput() -> RtoR::Monitor * override;
        };


        class BoundaryCondition : public Base::BoundaryConditions<RtoR::EquationState> {
            Real f, A;
            mutable jack_default_audio_sample_t *currentBuffer = nullptr;
            mutable size_t currentBufferLocation = 0;
            mutable size_t bufferSize = 0;
            mutable size_t bufferNumber = 0;

        public:
            BoundaryCondition(Real f, Real A);;
            void apply(EquationState &function, Real t) const override;
        };


        class CLI : public RtoRBCInterface {
            RealParameter freq =      RealParameter{1, "freq", "The freq of the driving force"};
            RealParameter amplitude =   RealParameter{1, "amplitude", "The amplitude of the driving force"};
            RealParameter damping =     RealParameter{1.5e-3, "damping", "The damping factor at the right hand region of the field"};
            RealParameter dampPercent = RealParameter{.2, "damp_percent", "Percentage (in the range 0..1) of space to use for damping"};
        public:
            CLI();
            auto getBoundary() const -> const void * override;

        protected:
            auto buildOpenGLOutput() -> RtoR::Monitor * override;
        };



    }
}

#endif //STUDIOSLAB_SIGNAL_H

//
// Created by joao on 4/1/23.
//

#ifndef STUDIOSLAB_SIGNAL_H
#define STUDIOSLAB_SIGNAL_H


#include <jack/types.h>
#include "Fields/Mappings/RtoR/Controller/RtoRBCInterface.h"

#include "Fields/Mappings/RtoR/View/OutputStructureBuilderRtoR.h"

#include "Fields/Mappings/RtoR/Model/RtoRFieldState.h"

#include "Fields/Mappings/RtoR/Model/FunctionsCollection/AnalyticOscillon.h"
#include "Fields/Mappings/RtoR/Model/FunctionsCollection/NullFunction.h"


#include "Phys/Numerics/Allocator.h"
#include "Phys/DifferentialEquations/BoundaryConditions.h"



const auto pi = 3.1415926535897932384626;

namespace RtoR {

    namespace Signal {
        extern Real xInitDampCutoff_normalized;
        extern Real dampFactor;


        class JackOutput : public OutputChannel {

        public:
            JackOutput();

            auto shouldOutput(double t, unsigned long timestep) -> bool override;

        private:

            void _out(const OutputPacket &outputPacket) override;
        };


        class OutputBuilder : public OutputStructureBuilderRtoR {
        protected:
        public:
            auto build(String outputFileName) -> OutputManager * override;

        protected:
            auto buildOpenGLOutput() -> RtoR::OutputOpenGL * override;
        };


        class BoundaryCondition : public Base::BoundaryConditions<RtoR::FieldState> {
            Real f, A;
            mutable jack_default_audio_sample_t *currentBuffer = nullptr;
            mutable size_t currentBufferLocation = 0;
            mutable size_t bufferSize = 0;
            mutable size_t bufferNumber = 0;

        public:
            BoundaryCondition(double f, double A);;
            void apply(FieldState &function, Real t) const override;
        };


        class CLI : public RtoRBCInterface {
            DoubleParameter freq =      DoubleParameter{1, "freq", "The freq of the driving force"};
            DoubleParameter amplitude =   DoubleParameter{1, "amplitude", "The amplitude of the driving force"};
            DoubleParameter damping =     DoubleParameter{1.5e-3, "damping", "The damping factor at the right hand region of the field"};
            DoubleParameter dampPercent = DoubleParameter{.2, "damp_percent", "Percentage (in the range 0..1) of space to use for damping"};
        public:
            CLI();
            auto getBoundary() const -> const void * override;
        };



    }
}

#endif //STUDIOSLAB_SIGNAL_H

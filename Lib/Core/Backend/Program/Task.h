//
// Created by joao on 30/08/2021.
//

#ifndef V_SHAPE_PROGRAM_H
#define V_SHAPE_PROGRAM_H

#include <Utils/Types.h>

namespace Slab::Core {
    class Task {
    public:
        struct CycleOptions {
            const enum Options {
                Cycle_nCycles,
                CycleUntilOutput,
                cycleCycleUntilFinished
            } cycleOption = Cycle_nCycles;

            const size_t nCycles = 1;

            CycleOptions(Options opts = Cycle_nCycles, size_t nCycles = 1)
                    : cycleOption(opts), nCycles(nCycles) {}

            CycleOptions(size_t nCycles)
                    : cycleOption(Options::Cycle_nCycles), nCycles(nCycles) {}
        };

        virtual ~Task() = default;

        virtual bool cycle(CycleOptions options) = 0;
    };

}

#endif //V_SHAPE_PROGRAM_H

//
// Created by joao on 8/30/25.
//

#include "DummyOutput.h"

namespace Slab::Math
{
    auto FDummyOutput::HandleOutput(const OutputPacket&) -> void
    {
    }

    FDummyOutput::FDummyOutput() : FOutputChannel("Dummy Output", 10, "Dummy output that serves the purpose of requiring NumericTask to update every provided number of steps.")
    {
    }
}

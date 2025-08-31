//
// Created by joao on 20/09/23.
//

#ifndef STUDIOSLAB_SNAPSHOTOUTPUT_H
#define STUDIOSLAB_SNAPSHOTOUTPUT_H

#include "Math/Function/RtoR/Model/RtoRNumericFunctionCPU.h"

#include "Math/Numerics/OutputChannel.h"

#include <fstream>


namespace Slab::Models::KGRtoR {

    using namespace Slab::Math;

    class SnapshotOutput : public FOutputChannel {
        Str outputFileName;

    protected:
        auto HandleOutput(const FOutputPacket &packet) -> void override;

        virtual auto filterData(const FOutputPacket &packet) -> RtoR::NumericFunction_CPU;

        SnapshotOutput(const Str &fileName, const Str &socketName, const Str &description);
    public:
        explicit SnapshotOutput(const Str &fileName);

        auto NotifyIntegrationHasFinished(const FOutputPacket &theVeryLastOutputInformation) -> bool override;

        static bool
        OutputNumericFunction(const Math::DiscreteSpace&, const Str& filename, const Vector<Pair<Str,Str>>& xtraPyDictEntries={});
    };

} // KleinGordon::RtoR

#endif //STUDIOSLAB_SNAPSHOTOUTPUT_H

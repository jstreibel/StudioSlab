//
// Created by joao on 20/09/23.
//

#ifndef STUDIOSLAB_SNAPSHOTOUTPUT_H
#define STUDIOSLAB_SNAPSHOTOUTPUT_H

#include "Math/Function/RtoR/Model/RtoRNumericFunctionCPU.h"

#include "Math/Numerics/Socket.h"

#include <fstream>


namespace Slab::Models::KGRtoR {

    using namespace Slab::Math;

    class SnapshotOutput : public Socket {
        Str outputFileName;

    protected:
        auto handleOutput(const OutputPacket &packet) -> void override;

        virtual auto filterData(const OutputPacket &packet) -> RtoR::NumericFunction_CPU;

        SnapshotOutput(const Str &fileName, const Str &socketName, const Str &description);
    public:
        SnapshotOutput(const Str &fileName);

        auto notifyIntegrationHasFinished(const OutputPacket &theVeryLastOutputInformation) -> bool override;

        static bool
        OutputNumericFunction(const Pointer<RtoR::NumericFunction>&, const Str& filename, Vector<Pair<Str,Str>> xtraPyDictEntries={});
    };

} // KleinGordon::RtoR

#endif //STUDIOSLAB_SNAPSHOTOUTPUT_H

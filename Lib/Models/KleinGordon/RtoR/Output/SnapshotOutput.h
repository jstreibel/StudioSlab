//
// Created by joao on 20/09/23.
//

#ifndef STUDIOSLAB_SNAPSHOTOUTPUT_H
#define STUDIOSLAB_SNAPSHOTOUTPUT_H

#include "Math/Function/RtoR/Model/RtoRDiscreteFunctionCPU.h"

#include "Math/Numerics/Output/Plugs/Socket.h"

#include <fstream>


namespace Slab::Math::RtoR {

    class SnapshotOutput : public Socket {
        Str outputFileName;

    protected:
        auto handleOutput(const OutputPacket &packet) -> void override;

        virtual auto filterData(const OutputPacket &packet) -> RtoR::DiscreteFunction_CPU;

        SnapshotOutput(const NumericConfig &config, const Str &fileName, const Str &socketName, const Str &description);
    public:
        SnapshotOutput(const NumericConfig &config, const Str &fileName);

        auto notifyIntegrationHasFinished(const OutputPacket &theVeryLastOutputInformation) -> bool override;
    };

} // KleinGordon::RtoR

#endif //STUDIOSLAB_SNAPSHOTOUTPUT_H

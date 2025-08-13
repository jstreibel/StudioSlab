//
// Created by joao on 15/12/24.
//

#ifndef STUDIOSLAB_KG2DSNAPSHOTOUTPUT_H
#define STUDIOSLAB_KG2DSNAPSHOTOUTPUT_H

#include "Math/Numerics/Socket.h"


namespace Slab::Models::KGR2toR {

    class KG2DSnapshotOutput : public Math::Socket {
        Str filename;

    protected:
        auto HandleOutput(const Math::OutputPacket &packet) -> void override;

    public:
        explicit KG2DSnapshotOutput(Str filename);

        auto notifyIntegrationHasFinished(const Math::OutputPacket &theVeryLastOutputInformation) -> bool override;
    };

}


#endif //STUDIOSLAB_KG2DSNAPSHOTOUTPUT_H

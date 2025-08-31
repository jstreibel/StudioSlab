//
// Created by joao on 15/12/24.
//

#ifndef STUDIOSLAB_KG2DSNAPSHOTOUTPUT_H
#define STUDIOSLAB_KG2DSNAPSHOTOUTPUT_H

#include "Math/Numerics/OutputChannel.h"


namespace Slab::Models::KGR2toR {

    class KG2DSnapshotOutput : public Math::FOutputChannel {
        Str filename;

    protected:
        auto HandleOutput(const Math::FOutputPacket &packet) -> void override;

    public:
        explicit KG2DSnapshotOutput(Str filename);

        auto NotifyIntegrationHasFinished(const Math::FOutputPacket &theVeryLastOutputInformation) -> bool override;
    };

}


#endif //STUDIOSLAB_KG2DSNAPSHOTOUTPUT_H

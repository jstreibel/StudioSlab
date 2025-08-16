//
// Created by joao on 10/14/24.
//

#include "Payload.h"

namespace Slab::Core {

    FPayload FPayload::CommandLineParsingFinished = FPayload{"CommandLineParsingFinished"};;
    FPayload FPayload::AllCommandLineParsingFinished = FPayload{"All CommandLineParsingFinished"};;

    FPayload::FPayload(FPayloadData Data) : Data(Data)
    {
    }

    bool FPayload::operator==(const FPayload &Other) const
    {
        return Data == Other.Data;
    }

    bool FPayload::operator==(const FPayloadData &PayloadData) const
    {
        return Data == PayloadData;
    }
} // Slab::Core
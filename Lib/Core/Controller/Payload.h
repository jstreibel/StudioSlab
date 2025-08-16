//
// Created by joao on 10/14/24.
//

#ifndef STUDIOSLAB_REQUEST_H
#define STUDIOSLAB_REQUEST_H

#include "Utils/String.h"

namespace Slab::Core {

    using FPayloadData = Str;

    class FPayload {
    public:
        explicit FPayload(FPayloadData);

        bool operator==(const FPayload&) const;

        bool operator==(const FPayloadData&) const;

        static FPayload CommandLineParsingFinished;
        static FPayload AllCommandLineParsingFinished;

    private:
        FPayloadData Data;
    };




} // Slab::Core

#endif //STUDIOSLAB_REQUEST_H

//
// Created by joao on 26/05/24.
//

#ifndef STUDIOSLAB_DATASET_H
#define STUDIOSLAB_DATASET_H

#include "Utils/String.h"
#include "Utils/Pointer.h"

namespace Slab::Math {

    class DataSet {
        Str type;
        Pointer<void> data;

    public:
        DataSet(Pointer<void> data, Str type);

        Str getType() const;

        auto getData() const -> Pointer<void>;
    };

    DefinePointer(DataSet)

} // Slab::Math

#endif //STUDIOSLAB_DATASET_H

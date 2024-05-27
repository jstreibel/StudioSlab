//
// Created by joao on 26/05/24.
//

#include "DataSet.h"

namespace Slab::Math {
    DataSet::DataSet(Pointer<void> data, Str type) : data(data), type(type) {

    }

    Str DataSet::getType() const {
        return type;
    }

    auto DataSet::getData() const -> Pointer<void> {
        return data;
    }
} // Slab::Math
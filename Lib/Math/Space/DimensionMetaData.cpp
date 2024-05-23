//
// Created by joao on 13/05/24.
//

#include "DimensionMetaData.h"

Slab::UInt Slab::Math::DimensionMetaData::computeFullDiscreteSize() const {
    UInt size = 1;
    for(auto n : N){
        size *= n;
    }

    return size;
}
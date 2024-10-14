//
// Created by joao on 26/05/24.
//

#include "DataSet.h"

namespace Slab::Math {
    DataSet::DataSet(DataType type) : type(type) {
    }

    Str DataSet::getType() const {
        return type;
    }

    NumericR2toRDataSet::NumericR2toRDataSet(const Pointer<R2toR::NumericFunction> &theData)
    : DataSetTemplate("Data:Numeric:R2toR", theData) {}
} // Slab::Math
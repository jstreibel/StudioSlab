//
// Created by joao on 26/05/24.
//

#ifndef STUDIOSLAB_DATASET_H
#define STUDIOSLAB_DATASET_H

#include "Utils/String.h"
#include "Utils/Pointer.h"
#include "Math/Function/R2toR/Model/R2toRNumericFunction.h"

namespace Slab::Math {

    using DataType = Str;

    class DataSet {
        DataType type;

    public:
        DataSet(DataType);

        DataType getType() const;

        virtual auto
        getData() const -> void* = 0;
    };

    DefinePointers(DataSet)

    template<class DataClass>
    class DataSetTemplate : public DataSet {
        Pointer<DataClass> data;
        DataSetTemplate() = delete;
    public:
        DataSetTemplate(DataType type, Pointer<DataClass> the_data) : DataSet(type), data(the_data)  {
        }

        auto getData() const -> void * override {
            return dynamic_cast<void*>(&(*data));
        }
    };

    class NumericR2toRDataSet : public DataSetTemplate<R2toR::NumericFunction> {
    public:
        explicit NumericR2toRDataSet(const Pointer<R2toR::NumericFunction> &theData);
    };

} // Slab::Math

#endif //STUDIOSLAB_DATASET_H

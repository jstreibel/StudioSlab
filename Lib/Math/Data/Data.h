//
// Created by joao on 10/18/24.
//

#ifndef STUDIOSLAB_DATA_H
#define STUDIOSLAB_DATA_H

#include "Utils/Types.h"

namespace Slab::Math {

    class Data;

    using UniqueID = CountType;
    using DataType = Str;
    using DataName = Str;
    using DataChangeCallback = std::function<void(TPointer<Data>)>;

    enum DataBehavior {
        MutableData,
        StaticData
    };

    class Data {
        friend class DataManager;

        static CountType n;
        const UniqueID id;
        DataName data_name;

    public:
        explicit Data(DataName);

        UniqueID
        get_id() const;

        DataName
        get_data_name() const;

        void
        change_data_name(DataName);

        virtual DataType
        get_data_type() const = 0;

        bool operator<(const Data &rhs) const;
        bool operator>(const Data &rhs) const;
        bool operator<=(const Data &rhs) const;
        bool operator>=(const Data &rhs) const;
    };

    class DataWrap {
        TVolatile<Data> data;

    public:
        DataWrap() = default;
        explicit DataWrap(TVolatile<Data>);

        UniqueID get_id() const;
        DataName get_name() const;
        DataType get_type() const;

        virtual auto
        get_data() const -> TPointer<Data>;

        bool is_valid() const;

        bool operator<(const DataWrap &rhs) const;

        bool operator>(const DataWrap &rhs) const;

        bool operator<=(const DataWrap &rhs) const;

        bool operator>=(const DataWrap &rhs) const;
    };

} // Slab::Math

#endif //STUDIOSLAB_DATA_H

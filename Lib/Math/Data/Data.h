//
// Created by joao on 10/18/24.
//

#ifndef STUDIOSLAB_DATA_H
#define STUDIOSLAB_DATA_H

#include "Utils/Types.h"

namespace Slab::Math {

    class Data;

    using UniqueID = Count;
    using DataType = Str;
    using DataName = Str;
    using DataChangeCallback = std::function<void(Pointer<Data>)>;

    enum DataBehavior {
        MutableData,
        StaticData
    };

    class Data {
        friend class DataManager;

        static Count n;
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
        Volatile<Data> data;

    public:
        DataWrap() = default;
        explicit DataWrap(Volatile<Data>);

        UniqueID get_id() const;
        DataName get_name() const;
        DataType get_type() const;

        virtual auto
        get_data() const -> Pointer<Data>;

        bool is_valid() const;

        bool operator<(const DataWrap &rhs) const;

        bool operator>(const DataWrap &rhs) const;

        bool operator<=(const DataWrap &rhs) const;

        bool operator>=(const DataWrap &rhs) const;
    };

} // Slab::Math

#endif //STUDIOSLAB_DATA_H

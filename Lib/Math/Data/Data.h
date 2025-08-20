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
    using FDataName = Str;
    using DataChangeCallback = std::function<void(TPointer<Data>)>;

    enum DataBehavior {
        MutableData,
        StaticData
    };

    class Data {
        friend class FDataRegistry;

        static CountType n;
        const UniqueID id;
        FDataName data_name;

    public:
        virtual ~Data() = default;
        explicit Data(FDataName);

        [[nodiscard]] UniqueID
        get_id() const;

        [[nodiscard]] FDataName
        get_data_name() const;

        void
        change_data_name(FDataName);

        virtual DataType
        get_data_type() const = 0;

        virtual
        float get_data_size_MiB() = 0;

        bool operator<(const Data &rhs) const;
        bool operator>(const Data &rhs) const;
        bool operator<=(const Data &rhs) const;
        bool operator>=(const Data &rhs) const;
    };

    class FDataWrap {
        TVolatile<Data> data;

    public:
        virtual ~FDataWrap() = default;
        FDataWrap() = default;
        explicit FDataWrap(TVolatile<Data>);

        [[nodiscard]] UniqueID get_id() const;
        [[nodiscard]] FDataName get_name() const;
        [[nodiscard]] DataType get_type() const;

        virtual auto
        GetData() const -> TPointer<Data>;

        bool is_valid() const;

        bool operator<(const FDataWrap &rhs) const;

        bool operator>(const FDataWrap &rhs) const;

        bool operator<=(const FDataWrap &rhs) const;

        bool operator>=(const FDataWrap &rhs) const;
    };

} // Slab::Math

#endif //STUDIOSLAB_DATA_H

//
// Created by joao on 10/18/24.
//

#include "Data.h"
#include "DataManager.h"

namespace Slab::Math {
    CountType Data::n = 0;

    Data::Data(FDataName name) : id(name=="[invalid]" ? 0 : ++n /* zero is reserved */), data_name(name) {}

    UniqueID Data::get_id() const { return id; }

    FDataName Data::get_data_name() const { return data_name; }

    void Data::change_data_name(FDataName new_name) {
        DataRegistry::ChangeDataName(data_name, new_name);
    }


    bool Data::operator<(const Data &rhs) const { return data_name < rhs.data_name; }

    bool Data::operator>(const Data &rhs) const { return rhs < *this; }

    bool Data::operator<=(const Data &rhs) const { return !(rhs < *this); }

    bool Data::operator>=(const Data &rhs) const { return !(*this < rhs); }





    FDataWrap::FDataWrap(TVolatile<Data> data) : data(data) { }

    UniqueID FDataWrap::get_id() const {
        if(!is_valid()) return 0;

        return data.lock()->get_id();
    }

    FDataName FDataWrap::get_name() const {
        if(!is_valid()) return "";

        return data.lock()->get_data_name();
    }

    DataType FDataWrap::get_type() const {
        if(!is_valid()) return "";

        return data.lock()->get_data_type();
    }

    auto FDataWrap::GetData() const -> TPointer<Data> {
        if(!is_valid()) return nullptr;

        return data.lock();
    }

    bool FDataWrap::is_valid() const {
        return !data.expired();
    }

    bool FDataWrap::operator<(const FDataWrap &rhs) const {
        if(!is_valid() || !rhs.is_valid()) return false;

        return *data.lock() < *rhs.data.lock();
    }

    bool FDataWrap::operator>(const FDataWrap &rhs) const {
        return rhs < *this;
    }

    bool FDataWrap::operator<=(const FDataWrap &rhs) const {
        return !(rhs < *this);
    }

    bool FDataWrap::operator>=(const FDataWrap &rhs) const {
        return !(*this < rhs);
    }




} // Slab::Math
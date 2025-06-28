//
// Created by joao on 10/18/24.
//

#include "Data.h"
#include "DataManager.h"

namespace Slab::Math {
    CountType Data::n = 0;

    Data::Data(DataName name) : id(name=="[invalid]" ? 0 : ++n /* zero is reserved */), data_name(name) {}

    UniqueID Data::get_id() const { return id; }

    DataName Data::get_data_name() const { return data_name; }

    void Data::change_data_name(DataName new_name) {
        DataManager::ChangeDataName(data_name, new_name);
    }


    bool Data::operator<(const Data &rhs) const { return data_name < rhs.data_name; }

    bool Data::operator>(const Data &rhs) const { return rhs < *this; }

    bool Data::operator<=(const Data &rhs) const { return !(rhs < *this); }

    bool Data::operator>=(const Data &rhs) const { return !(*this < rhs); }





    DataWrap::DataWrap(Volatile<Data> data) : data(data) { }

    UniqueID DataWrap::get_id() const {
        if(!is_valid()) return 0;

        return data.lock()->get_id();
    }

    DataName DataWrap::get_name() const {
        if(!is_valid()) return "";

        return data.lock()->get_data_name();
    }

    DataType DataWrap::get_type() const {
        if(!is_valid()) return "";

        return data.lock()->get_data_type();
    }

    auto DataWrap::get_data() const -> Pointer<Data> {
        if(!is_valid()) return nullptr;

        return data.lock();
    }

    bool DataWrap::is_valid() const {
        return !data.expired();
    }

    bool DataWrap::operator<(const DataWrap &rhs) const {
        if(!is_valid() || !rhs.is_valid()) return false;

        return *data.lock() < *rhs.data.lock();
    }

    bool DataWrap::operator>(const DataWrap &rhs) const {
        return rhs < *this;
    }

    bool DataWrap::operator<=(const DataWrap &rhs) const {
        return !(rhs < *this);
    }

    bool DataWrap::operator>=(const DataWrap &rhs) const {
        return !(*this < rhs);
    }




} // Slab::Math
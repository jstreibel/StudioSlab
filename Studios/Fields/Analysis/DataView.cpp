//
// Created by joao on 7/1/24.
//

#include "DataView.h"

namespace Studios::Fields {
    DataView::DataView(DataView::DataFunction func) : function(func) {

    }

    void DataView::setData(DataView::DataFunction func) { function = func; }
} // Fields
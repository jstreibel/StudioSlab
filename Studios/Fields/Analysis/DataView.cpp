//
// Created by joao on 7/1/24.
//

#include "DataView.h"

namespace Studios::Fields {
    FDataView::FDataView(FDataView::DataFunction func) : function(func) {

    }

    void FDataView::setData(FDataView::DataFunction func) { function = func; }
} // Fields
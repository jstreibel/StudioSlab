//
// Created by joao on 11/4/21.
//

#include "Measure.h"

Measure::Measure(RealVector linearValues) : linearValues(linearValues) {

}

Real Measure::operator*() {
    auto v = 1.;
    for(auto lv : linearValues)
         v *= lv;

    return v;
}

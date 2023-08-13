//
// Created by joao on 8/4/23.
//

#include "ThermalBuilder.h"
#include "Models/KleinGordon/KGBuilder.h"

namespace RtoR::Thermal {

class Builder : public Fields::KleinGordon::KGBuilder {
public:
    void *getHamiltonian() override {
        return nullptr;
    }
};

}
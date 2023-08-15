//
// Created by joao on 8/4/23.
//

#ifndef STUDIOSLAB_THERMALBUILDER_H
#define STUDIOSLAB_THERMALBUILDER_H

#include "Models/KleinGordon/RtoR/KG-RtoRBuilder.h"

namespace RtoR::Thermal {

    class Builder : public RtoR::KGBuilder {
        RealParameter temperature = RealParameter(.0, "T,temperature", "The Langevin temperature reservoir's temperature");
        RealParameter dissipation = RealParameter(.0, "k,dissipation_coefficient", "The dynamics dissipation coefficient");

    public:
        Builder(const Str &name, const Str &generalDescription, bool doRegister=false);

    protected:
        auto buildOpenGLOutput() -> Monitor * override;

    public:

        auto buildEquationSolver() -> void * override;

        auto suggestFileName() const -> Str override;

    };
}


#endif //STUDIOSLAB_THERMALBUILDER_H

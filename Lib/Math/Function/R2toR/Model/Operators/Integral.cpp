//
// Created by joao on 10/11/24.
//

#include "Integral.h"

namespace Slab::Math::R2toR {
    Integral::Integral(Integral::Config config) : config(config) {}

    Pointer<RtoR::NumericFunction_CPU>
    Integral::operator[](Pointer<NumericFunction_CPU> func) {
        if(config.direction!=Config::dx || !config.compute_avg) NOT_IMPLEMENTED

        IN data = func->getSpace().getHostData(true);
        fix N = func->getN();
        fix M = func->getM();
        fix invM = 1./(Real)M;

        fix t_min = func->getDomain().yMin;
        fix t_max = func->getDomain().yMax;
        auto output = DataAlloc<RtoR::NumericFunction_CPU>("∫" + func->get_data_name() + " dy", M, t_min, t_max);
        OUT out_data = output->getSpace().getHostData(true);

        for(int j=0; j<M; ++j) {
            auto &avg = out_data[j];
            avg = .0;
            IN curr_in = &data[j*N];
            for (int i = 0; i < N; ++i)
                avg += curr_in[i];

            avg *= invM;
        }

        return output;
    }


} // Slab::Math::R2toR
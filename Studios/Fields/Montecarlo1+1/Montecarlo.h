//
// Created by joao on 3/17/23.
//

#ifndef STUDIOSLAB_MONTECARLO_H
#define STUDIOSLAB_MONTECARLO_H

#include "Monitor.h"

#include "Mappings/RtoR/Model/RtoRFunction.h"
#include "Models/KleinGordon/RtoR/KG-RtoRBoundaryCondition.h"
#include "Mappings/RtoR/Model/FunctionsCollection/RegularDiracDelta.h"
#include "Mappings/RtoR/Model/FunctionsCollection/NullFunction.h"

#include "Utils/RandUtils.h"
#include "Models/KleinGordon/RtoR/KG-RtoRBuilder.h"


namespace Montecarlo {

    class Input : public RtoRBCInterface {
        RealParameter T = RealParameter(1.e1, "Temperature,T", "System temperature");
        RealParameter E = RealParameter{1., "E", "System energy"};
        IntegerParameter n = IntegerParameter{100, "n", "Number of initial oscillons"};

    protected:
        auto buildOpenGLOutput() -> RtoR::Monitor * override { return new Montecarlo::Monitor(numericParams); }

    public:
        Input()
        : RtoRBCInterface("Montecarlo builder", "Simulation builder for 1+1 dim Montecarlo simulation.")
        {
            interface->addParameters({&T, &E, &n});
        }

        auto getBoundary() -> void * override{

            auto L = numericParams.getL(); // not good bc 'L' is not my parameter.
            auto xLeft = numericParams.getxMin();
            //auto L = 20.;

            auto oscLength = L / *n;
            auto oscEnergy = *E / *n;

            auto osc_eps = oscLength/2;
            ////auto osc_eps = a*a / (3*oscEnergy);
            auto a = sqrt(3*osc_eps*oscEnergy);
            auto a0= .5*a;


            RtoR::FunctionSummable dPhidt0;

            const auto ONE_plus = 1.+1.e-10;
            for(int i=0; i<*n; i++){
                auto tx = xLeft + ONE_plus*Real(i)*(oscLength) + osc_eps;

                auto s = RandUtils::RandInt()%2?1.:-1.;

                dPhidt0 += RtoR::RegularDiracDelta(osc_eps, s*a, RtoR::RegularDiracDelta::Triangle, tx);
            }

            return new RtoR::BoundaryCondition(RtoR::NullFunction().Clone(), dPhidt0.Clone());

        }

    };




}


#endif //STUDIOSLAB_MONTECARLO_H

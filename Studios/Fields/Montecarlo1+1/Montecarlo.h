//
// Created by joao on 3/17/23.
//

#ifndef STUDIOSLAB_MONTECARLO_H
#define STUDIOSLAB_MONTECARLO_H


#include "Mappings/RtoR/Controller/RtoRBCInterface.h"

#include "RtoROutGLMontecarlo.h"

#include "Mappings/RtoR/Model/RtoRFunction.h"
#include "Mappings/RtoR/Model/RtoRBoundaryCondition.h"
#include "Mappings/RtoR/Model/FunctionsCollection/RegularDiracDelta.h"
#include "Mappings/RtoR/Model/FunctionsCollection/NullFunction.h"

#include "Phys/Thermal/Utils/RandUtils.h"
#include "Phys/Numerics/Allocator.h"
#include "Mappings/RtoR/Core/RtoRModelAllocator_Montecarlo.h"


namespace Montecarlo {



    class OutputBuilder : public OutputStructureBuilderRtoR {
    protected:
        auto buildOpenGLOutput() -> RtoR::OutputOpenGL * override { return new Montecarlo::OutGL( ); }
    public:
        OutputBuilder() : OutputStructureBuilderRtoR("Montecarlo output builder", "R to R monetcarlo sim") {}
    };



    class Input : public RtoRBCInterface {
        RealParameter T = RealParameter(1.e4, "Temperature,T", "System temperature");
        RealParameter E = RealParameter{1., "E", "System energy"};
        IntegerParameter n = IntegerParameter{100, "n", "Number of initial oscillons"};

    public:
        Input()
        : RtoRBCInterface("Montecarlo builder", "Simulation builder for 1+1 dim Montecarlo simulation.",
                          BuilderBasePtr(new Montecarlo::OutputBuilder()))
        {
            interface->addParameters({&T, &E, &n});
        }

    private:
        auto getBoundary() const -> const void * override{

            auto L = Numerics::Allocator::GetInstance().getNumericParams().getL(); // not good bc 'L' is not my parameter.
            auto xLeft = Numerics::Allocator::GetInstance().getNumericParams().getxLeft();
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

    public:
        auto registerAllocator() const -> void override {
            Numerics::Allocator::Initialize<RtoRModelAllocator_Montecarlo>();
        }
    };




}


#endif //STUDIOSLAB_MONTECARLO_H

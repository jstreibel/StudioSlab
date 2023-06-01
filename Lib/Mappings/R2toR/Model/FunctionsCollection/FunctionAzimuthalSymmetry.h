//
// Created by joao on 23/09/2019.
//

#ifndef V_SHAPE_FUNCTIONAZIMUTHALSYMMETRY_H
#define V_SHAPE_FUNCTIONAZIMUTHALSYMMETRY_H


#include "Mappings/R2toR/Model/R2toRFunction.h"
#include "Mappings/RtoR/Model/RtoRFunction.h"
#include <cmath>
#include "Mappings/R2toR/Model/Transform.h"


namespace R2toR {

    class FunctionAzimuthalSymmetry : public Function {
    public:
        FunctionAzimuthalSymmetry(const RtoR::Function *baseFunction, Real coef=1, Real eccentricity=0,
                                  Real angle=0, bool keepBaseFunction=true)
            : radialFunction(*baseFunction), c(coef), theta(angle), a(1. / sqrt(1. - eccentricity * eccentricity)), keepBaseFunction(keepBaseFunction)
               {};
        ~FunctionAzimuthalSymmetry(){ if(keepBaseFunction) delete &radialFunction; }

        Real operator()(Real2D x) const override {
            Rotation T(theta);
            Real2D Tx = T*x;
            Tx.x /= a;
            const Real r = sqrt(Tx.x*Tx.x + Tx.y*Tx.y);

            return c * radialFunction(r);
        };

        const RtoR::Function &getRadialFunction() const { return radialFunction; }

        String myName() const override {
            return "2d azimuthal-symmetric";
        }


    private:
        const Real c;
        const Real a;
        const Real theta;
        const bool keepBaseFunction;
        const RtoR::Function &radialFunction;
    };

}


#endif //V_SHAPE_FUNCTIONAZIMUTHALSYMMETRY_H

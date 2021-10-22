//
// Created by joao on 23/09/2019.
//

#ifndef V_SHAPE_FUNCTIONAZIMUTHALSYMMETRY_H
#define V_SHAPE_FUNCTIONAZIMUTHALSYMMETRY_H


#include "Lib/Fields/Maps/R2toR/Model/R2toRFunction.h"
#include "Lib/Fields/Maps/RtoR/Model/RtoRFunction.h"
#include <cmath>
#include <Lib/Fields/Maps/R2toR/Model/Transform.h>


namespace R2toR {

    class FunctionAzimuthalSymmetry : public Function {
    public:
        FunctionAzimuthalSymmetry(const RtoR::Function *baseFunction, Real coef, Real eccentricity=0., Real angle=0.,
                                  bool keepBaseFunction=true)
            : baseFunction(*baseFunction), c(coef), theta(angle), a(1./sqrt(1.-eccentricity*eccentricity)), keepBaseFunction(keepBaseFunction)
               {};
        ~FunctionAzimuthalSymmetry(){ if(keepBaseFunction) delete &baseFunction; }

        Real operator()(Real2D x) const override {
            Rotation T(theta);
            Real2D Tx = T*x;
            Tx.x /= a;
            const Real r = sqrt(Tx.x*Tx.x + Tx.y*Tx.y);

            return c*baseFunction(r);
        };

        const RtoR::Function &getBaseFunction() const { return baseFunction; }

    private:
        const Real c;
        const Real a;
        const Real theta;
        const bool keepBaseFunction;
        const RtoR::Function &baseFunction;
    };

}


#endif //V_SHAPE_FUNCTIONAZIMUTHALSYMMETRY_H

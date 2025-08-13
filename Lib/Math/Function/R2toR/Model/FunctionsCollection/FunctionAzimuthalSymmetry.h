//
// Created by joao on 23/09/2019.
//

#ifndef V_SHAPE_FUNCTIONAZIMUTHALSYMMETRY_H
#define V_SHAPE_FUNCTIONAZIMUTHALSYMMETRY_H


#include "Math/Function/R2toR/Model/R2toRFunction.h"
#include "Math/Function/RtoR/Model/RtoRFunction.h"
#include <cmath>
#include "Math/Function/R2toR/Model/Transform.h"


namespace Slab::Math::R2toR {

class FunctionAzimuthalSymmetry : public R2toR::Function {
    public:
        typedef std::shared_ptr<FunctionAzimuthalSymmetry> Ptr;

        FunctionAzimuthalSymmetry(const RtoR::Function *baseFunction, DevFloat coef=1, DevFloat eccentricity=0,
                                  DevFloat angle=0, bool keepBaseFunction=true)
            : radialFunction(*baseFunction), c(coef), theta(angle), a(1. / sqrt(1. - eccentricity * eccentricity)), keepBaseFunction(keepBaseFunction)
               {};
        ~FunctionAzimuthalSymmetry(){ if(keepBaseFunction) delete &radialFunction; }

        DevFloat operator()(Real2D x) const override {
            Rotation T(theta);
            Real2D Tx = T*x;
            Tx.x /= a;
            const DevFloat r = sqrt(Tx.x*Tx.x + Tx.y*Tx.y);

            return c * radialFunction(r);
        };

        const RtoR::Function &getRadialFunction() const { return radialFunction; }

        Str generalName() const override {
            return "2d azimuthal-symmetric";
        }


    private:
        const DevFloat c;
        const DevFloat a;
        const DevFloat theta;
        const bool keepBaseFunction;
        const RtoR::Function &radialFunction;
    };

}


#endif //V_SHAPE_FUNCTIONAZIMUTHALSYMMETRY_H

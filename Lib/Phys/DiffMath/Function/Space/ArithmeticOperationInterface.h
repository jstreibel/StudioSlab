//
// Created by joao on 06/09/2019.
//

#ifndef V_SHAPE_ARITHMETICOPSINTERFACE_H
#define V_SHAPE_ARITHMETICOPSINTERFACE_H

#include "Phys/DiffMath/CoreNativeTypes.h"

/** Arithmetical Operations Interface */

namespace Utils {
    template<class OPS_TYPE>
    class ArithmeticOpsInterface {
    public:
        virtual OPS_TYPE &Add(const OPS_TYPE &toi) = 0;

        virtual OPS_TYPE &StoreAddition(const OPS_TYPE &toi1, const OPS_TYPE &toi2) = 0;

        virtual OPS_TYPE &StoreSubtraction(const OPS_TYPE &aoi1, const OPS_TYPE &aoi2) = 0;

        virtual OPS_TYPE &Multiply(Real a) = 0;

        OPS_TYPE &operator+=(const OPS_TYPE &aoi) { return this->Add(aoi); }

        OPS_TYPE &operator*=(Real a) { return this->Multiply(a); }
    };
}


#endif //V_SHAPE_ARITHMETICOPSINTERFACE_H

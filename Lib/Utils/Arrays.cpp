//
// Created by joao on 25/09/23.
//

#include "Arrays.h"
#include "Types.h"
#include "Utils.h"


namespace Slab::Utils {

    MaxInfo GetMax(const RealArray &arr) {
        double maxVal = arr[0];
        std::size_t maxIdx = 0;

        for (std::size_t i = 1; i < arr.size(); ++i) {
            fix val = arr[i];

            if (val > maxVal) {
                maxVal = val;
                maxIdx = i;
            }
        }

        if(!Common::areEqual(maxVal, arr.max()))
            throw Exception(Str(__PRETTY_FUNCTION__) + " error.");

        return {maxVal, maxIdx};
    }


}
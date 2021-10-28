//
// Created by joao on 11/10/2019.
//

#ifndef V_SHAPE_SPACEOUTPUTFORMATTERBASE_H
#define V_SHAPE_SPACEOUTPUTFORMATTERBASE_H


#include "Studios/CoreMath/Numerics/Output/Util/FieldStateOutputInterface.h"
#include "Studios/CoreMath/Numerics/Output/Util/ByteData.h"

#include <Studios/CoreMath/Function/Space/DiscreteSpace.h>


class OutputFormatterBase {
public:
    virtual ~OutputFormatterBase() = default;

    virtual auto operator() (const DiscreteSpace &fOut) const -> ByteData = 0;

    virtual auto operator() (const Real &out) const -> ByteData = 0;

    virtual auto getFormatDescription() const -> String = 0;
};


#endif //V_SHAPE_SPACEOUTPUTFORMATTERBASE_H

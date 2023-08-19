//
// Created by joao on 11/10/2019.
//

#ifndef V_SHAPE_SPACEOUTPUTFORMATTERBASE_H
#define V_SHAPE_SPACEOUTPUTFORMATTERBASE_H


#include <Phys/Numerics/Output/Util/FieldStateOutputInterface.h>
#include <Phys/Numerics/Output/Util/ByteData.h>

#include <Phys/Space/Impl/DiscreteSpace.h>


class OutputFormatterBase {
public:
    virtual ~OutputFormatterBase() = default;

    virtual auto operator() (const DiscreteSpace &fOut) const -> Numerics::ByteData = 0;

    virtual auto operator() (const Real &out) const -> Numerics::ByteData = 0;

    virtual auto isBinary() const -> bool = 0;

    virtual auto getFormatDescription() const -> Str = 0;
};


#endif //V_SHAPE_SPACEOUTPUTFORMATTERBASE_H

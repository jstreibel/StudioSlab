//
// Created by joao on 11/10/2019.
//

#ifndef V_SHAPE_SPACEOUTPUTFORMATTERBASE_H
#define V_SHAPE_SPACEOUTPUTFORMATTERBASE_H


#include <Math/Numerics/Output/Util/FieldStateOutputInterface.h>
#include <Math/Numerics/Output/Util/ByteData.h>

#include <Math/Space/Impl/DiscreteSpace.h>


namespace Slab::Math {

    class OutputFormatterBase {
    public:
        virtual ~OutputFormatterBase() = default;

        virtual auto operator()(const DiscreteSpace &fOut) const -> ByteData = 0;

        virtual auto operator()(const Real &out) const -> ByteData = 0;

        virtual auto isBinary() const -> bool = 0;

        virtual auto getFormatDescription() const -> Str = 0;
    };


}

#endif //V_SHAPE_SPACEOUTPUTFORMATTERBASE_H

//
// Created by joao on 11/10/2019.
//

#ifndef V_SHAPE_DISCRETESPACE_H
#define V_SHAPE_DISCRETESPACE_H


#include <utility>

#include "Utils/NativeTypes.h"
#include "Math/VectorSpace/Algebra/NumericAlgebra.h"
#include "Core/Tools/Log.h"
#include "Math/VectorSpace/DimensionMetaData.h"


namespace Slab::Math {

    class DiscreteSpaceCPU;

    class DiscreteSpace : public NumericAlgebra<DiscreteSpace> {
    public:
        using NumericAlgebra<DiscreteSpace>::operator=;


        explicit DiscreteSpace(const DimensionMetaData &dim);

        virtual ~DiscreteSpace();

        auto hostCopy(UInt maxResolution) const -> DiscreteSpaceCPU *;

        const DimensionMetaData &getMetaData() const { return dimensionMetaData; }

        UInt getTotalDiscreteSites() const;

        virtual
        auto dataOnGPU() const -> bool { return false; }

        virtual
        auto getDeviceData() const -> const DeviceVector & { throw "trying to access device data on host Space"; };

        virtual
        auto getDeviceData() -> DeviceVector & { throw "trying to access device data on host Space"; };

        auto getHostData(bool sync = false) const -> const RealArray &;

        auto getHostData(bool sync = false) -> RealArray &;

        // TODO colocar essa aqui no operator =
        virtual void setToValue(const DiscreteSpace &param) = 0;

    public:
        /*! Sync host with server information. */
        virtual void syncHost() const;

        virtual void upload();

    protected:
        const DimensionMetaData dimensionMetaData;
        RealArray data;
    };


}

#endif //V_SHAPE_DISCRETESPACE_H

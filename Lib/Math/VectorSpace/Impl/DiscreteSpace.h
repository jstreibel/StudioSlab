//
// Created by joao on 11/10/2019.
//

#ifndef V_SHAPE_DISCRETESPACE_H
#define V_SHAPE_DISCRETESPACE_H

#include "Math/VectorSpace/Algebra/NumericAlgebra.h"
#include "Core/Tools/Log.h"
#include "Math/VectorSpace/DimensionMetaData.h"

#ifdef USE_CUDA
#include "Utils/TypesGPU.h"
#else
class DeviceVector;
#endif

namespace Slab::Math {
    class DiscreteSpaceCPU;

    class DiscreteSpace : public NumericAlgebra<DiscreteSpace> {
    public:
        using NumericAlgebra<DiscreteSpace>::operator=;


        explicit DiscreteSpace(const DimensionMetaData &dim);

        virtual ~DiscreteSpace();

        [[nodiscard]] auto hostCopy(UInt maxResolution) const -> DiscreteSpaceCPU *;

        [[nodiscard]] const DimensionMetaData &getMetaData() const { return dimensionMetaData; }

        [[nodiscard]] UInt getTotalDiscreteSites() const;

        [[nodiscard]] virtual
        auto IsDataOnGPU() const -> bool { return false; }

        [[nodiscard]] virtual
        auto getDeviceData() const -> const DeviceVector & { throw Exception("trying to access device data on host Space"); };

        [[nodiscard]] virtual Device GetDevice() const = 0;

        virtual
        auto getDeviceData() -> DeviceVector & { throw Exception("trying to access device data on host Space"); };

        [[nodiscard]] auto getHostData(bool sync = false) const -> const RealArray &;

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

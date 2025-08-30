//
// Created by joao on 11/10/2019.
//

#ifndef V_SHAPE_DISCRETESPACEGPU_H
#define V_SHAPE_DISCRETESPACEGPU_H

#include "DiscreteSpace.h"
#include "Utils/TypesGPU.h"


namespace Slab::Math {

#if USE_CUDA

class DiscreteSpaceGPU : public DiscreteSpace
{

public:

    explicit DiscreteSpaceGPU(DimensionMetaData dim);
    ~DiscreteSpaceGPU() override;

    auto Add      (const DiscreteSpace &toi) -> DiscreteSpace & override;
    auto Subtract (const DiscreteSpace &toi) -> DiscreteSpace & override;
    auto Multiply (floatt a)                 -> DiscreteSpace & override;

    auto StoreAddition       (const DiscreteSpace &toi1, const DiscreteSpace &toi2) -> DiscreteSpace & override;
    auto StoreSubtraction    (const DiscreteSpace &aoi1, const DiscreteSpace &aoi2) -> DiscreteSpace & override;
    auto StoreScalarMultiplication (const DiscreteSpace &, DevFloat)                    -> DiscreteSpace & override;

    auto setToValue(const DiscreteSpace &param) -> void override;

    [[nodiscard]] auto IsDataOnGPU() const -> bool override;
    [[nodiscard]] Device GetDevice() const override;

    auto getDeviceData()       ->       DeviceVector & override;
    auto getDeviceData() const -> const DeviceVector & override;

    void notifyHostIsUpdated();

protected:

    void syncHost() const override;
    void upload() override;

private:

    bool hostIsUpdated = false;
    RealArray &XHost = data;
    DeviceVector XDev;
};

#else

class DiscreteSpaceGPU : public DiscreteSpace {
public:
    DiscreteSpaceGPU(DimensionMetaData dim) : DiscreteSpace(dim){
        throw "Error instantiating DiscreteSpaceGPU: program compiled with no GPU support.";
    };
    ~DiscreteSpaceGPU() {};

    DiscreteSpace &Add(const DiscreteSpace &toi) override { NOT_IMPLEMENTED_CLASS_METHOD };

    DiscreteSpace &StoreAddition(const DiscreteSpace &toi1, const DiscreteSpace &toi2) override { NOT_IMPLEMENTED_CLASS_METHOD };

    DiscreteSpace &StoreSubtraction(const DiscreteSpace &aoi1, const DiscreteSpace &aoi2) override { NOT_IMPLEMENTED_CLASS_METHOD };

    DiscreteSpace &Multiply(floatt a) override { NOT_IMPLEMENTED_CLASS_METHOD };
};
#endif


}

#endif //V_SHAPE_DISCRETESPACEGPU_H

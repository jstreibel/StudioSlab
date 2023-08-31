//
// Created by joao on 11/10/2019.
//

#ifndef V_SHAPE_DISCRETESPACEGPU_H
#define V_SHAPE_DISCRETESPACEGPU_H

#include "DiscreteSpace.h"
#include <Utils/DeviceConfig.h>


#if USE_CUDA || defined(__NVCC__) || defined(__CUDACC__)
class DiscreteSpaceGPU : public DiscreteSpace {
public:
    DiscreteSpaceGPU(DimensionMetaData dim, Real h);
    ~DiscreteSpaceGPU();

    auto Add      (const DiscreteSpace &toi) -> DiscreteSpace & override;
    auto Subtract (const DiscreteSpace &toi) -> DiscreteSpace & override;
    auto Multiply (floatt a)                 -> DiscreteSpace & override;

    auto StoreAddition       (const DiscreteSpace &toi1, const DiscreteSpace &toi2) -> DiscreteSpace & override;
    auto StoreSubtraction    (const DiscreteSpace &aoi1, const DiscreteSpace &aoi2) -> DiscreteSpace & override;
    auto StoreMultiplication (const DiscreteSpace &aoi1, const Real a)              -> DiscreteSpace & override;

    auto setToValue(const DiscreteSpace &param) -> void override;

    auto dataOnGPU() const -> bool override;

    auto getDeviceData()       ->       DeviceVector & override;
    auto getDeviceData() const -> const DeviceVector & override;

    void notifyHostIsUpdated();

protected:
    void syncHost() const override;
    void upload() override;

private:
    bool hostIsUpdated = false;
    RealVector &XHost = data;
    DeviceVector &XDev;
};
#else
class DiscreteSpaceGPU : public DiscreteSpace {
public:
    DiscreteSpaceGPU(DimensionMetaData dim, Real h) : DiscreteSpace(dim, h){
        throw "Error instatiating DiscreteSpaceGPU: program compiled with no GPU support.";
    };
    ~DiscreteSpaceGPU() {};

    DiscreteSpace &Add(const DiscreteSpace &toi) override { return *this; };

    DiscreteSpace &StoreAddition(const DiscreteSpace &toi1, const DiscreteSpace &toi2) override { return *this; };

    DiscreteSpace &StoreSubtraction(const DiscreteSpace &aoi1, const DiscreteSpace &aoi2) override { return *this; };

    DiscreteSpace &Multiply(floatt a) override { return *this; };
};
#endif


#endif //V_SHAPE_DISCRETESPACEGPU_H

//
// Created by joao on 11/10/2019.
//

#ifndef V_SHAPE_DISCRETESPACEGPU_H
#define V_SHAPE_DISCRETESPACEGPU_H

#include "DiscreteSpace.h"
#include <Common/DeviceConfig.h>


#if USE_CUDA
class DiscreteSpaceGPU : public DiscreteSpace {
public:
    DiscreteSpaceGPU(DimensionMetaData dim, Real h);
    ~DiscreteSpaceGPU();

    DiscreteSpace &Add(const DiscreteSpace &toi) override;

    DiscreteSpace &StoreAddition(const DiscreteSpace &toi1, const DiscreteSpace &toi2) override;

    DiscreteSpace &StoreSubtraction(const DiscreteSpace &aoi1, const DiscreteSpace &aoi2) override;

    DiscreteSpace &Multiply(floatt a) override;

    void setToValue(const DiscreteSpace &param) override;

    DeviceVector &getXDev();
    const DeviceVector &getXDev() const;

    void notifyHostIsUpdated();

protected:
    void syncHost() const override;

private:
    bool hostIsUpdated = false;
    VecFloat &XHost = X;
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

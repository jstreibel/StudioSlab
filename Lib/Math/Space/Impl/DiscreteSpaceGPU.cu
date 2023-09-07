//
// Created by joao on 11/10/2019.
//

#include "DiscreteSpaceGPU.h"

#include <Utils/Utils.h>

#include <thrust/iterator/constant_iterator.h>

DiscreteSpaceGPU::DiscreteSpaceGPU(DimensionMetaData dim, floatt h)
: DiscreteSpace(dim, h)
, XDev(*new DeviceVector(dim.computeFullDiscreteSize())) {

}

DiscreteSpaceGPU::~DiscreteSpaceGPU() {
    delete &XDev;
}

DiscreteSpace &DiscreteSpaceGPU::Add(const DiscreteSpace &inSpace) {
    auto& inSpaceGPU = dynamic_cast<const DiscreteSpaceGPU&>(inSpace);

    const auto &in = inSpace.getDeviceData();
    auto &out = this->getDeviceData();

    thrust::transform(in.begin(), in.end(),
                      out.begin(), out.begin(),
                      thrust::plus<Real>());

    hostIsUpdated = false;

    return *this;
}

DiscreteSpace &DiscreteSpaceGPU::Subtract(const DiscreteSpace &inSpace) {
    auto &inSpaceGPU = dynamic_cast<const DiscreteSpaceGPU &>(inSpace);

    auto &in = inSpaceGPU.XDev;
    auto &out = XDev;

    thrust::transform(in.begin(), in.end(),
                      out.begin(), out.begin(),
                      thrust::minus<Real>());

    hostIsUpdated = false;

    return *this;
}

DiscreteSpace &DiscreteSpaceGPU::StoreAddition(const DiscreteSpace &toi1, const DiscreteSpace &toi2) {
    cast(func1, const DiscreteSpaceGPU&, toi1);
    cast(func2, const DiscreteSpaceGPU&, toi2);

    auto &in1 = func1.XDev;
    auto &in2 = func2.XDev;
    auto &out = XDev;

    thrust::transform(in1.begin(), in1.end(),
                      in2.begin(),
                      out.begin(),
                      thrust::plus<Real>());

    hostIsUpdated = false;

    return *this;
}

DiscreteSpace &DiscreteSpaceGPU::StoreSubtraction(const DiscreteSpace &aoi1, const DiscreteSpace &aoi2) {
    cast(space1, const DiscreteSpaceGPU&, aoi1);
    cast(space2, const DiscreteSpaceGPU&, aoi2);
    auto &in1 = space1.getDeviceData();
    auto &in2 = space2.getDeviceData();
    auto &out = this->getDeviceData();

    thrust::transform(in1.begin(), in1.end(),
                      in2.begin(),
                      out.begin(),
                      thrust::minus<Real>());

    hostIsUpdated = false;

    return *this;
}

DiscreteSpace &DiscreteSpaceGPU::StoreMultiplication(const DiscreteSpace &aoi1, const Real a) {
    cast(space1, const DiscreteSpaceGPU&, aoi1);
    auto &in1 = space1.getDeviceData();
    auto &out = this->getDeviceData();

    thrust::transform(in1.begin(), in1.end(),
                      thrust::make_constant_iterator(a),
                      out.begin(),
                      thrust::multiplies<Real>());

    hostIsUpdated = false;

    return *this;
}

DiscreteSpace &DiscreteSpaceGPU::Multiply(floatt a) {
    auto &in = XDev;
    auto &out = XDev;

    thrust::transform(in.begin(), in.end(),
                      thrust::make_constant_iterator(a),
                      out.begin(),
                      thrust::multiplies<Real>());

    hostIsUpdated = false;

    return *this;
}

void DiscreteSpaceGPU::syncHost() const {
    if (!hostIsUpdated){
        DiscreteSpaceGPU &me = *const_cast<DiscreteSpaceGPU*>(this); // cheating!!!
        thrust::copy(XDev.begin(), XDev.end(), std::begin(XHost));
        me.hostIsUpdated = true;
    }
}

void DiscreteSpaceGPU::upload() {
    //DiscreteSpaceGPU &me = *const_cast<DiscreteSpaceGPU*>(this); // cheating!!!
    thrust::copy(std::begin(XHost), std::end(XHost), XDev.begin());
    this->hostIsUpdated = true;
}

void DiscreteSpaceGPU::setToValue(const DiscreteSpace &inSpace) {
    auto &in = dynamic_cast<const DiscreteSpaceGPU &>(inSpace).getDeviceData();
    thrust::copy(in.begin(), in.end(), XDev.begin());

    hostIsUpdated = false;
}

DeviceVector &DiscreteSpaceGPU::getDeviceData() {
    return XDev;
}

const DeviceVector &DiscreteSpaceGPU::getDeviceData() const {
    return XDev;
}

void DiscreteSpaceGPU::notifyHostIsUpdated() {
    hostIsUpdated = true;
}

bool DiscreteSpaceGPU::dataOnGPU() const {
    return true;
}




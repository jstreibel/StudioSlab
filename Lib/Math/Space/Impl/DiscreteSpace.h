//
// Created by joao on 11/10/2019.
//

#ifndef V_SHAPE_DISCRETESPACE_H
#define V_SHAPE_DISCRETESPACE_H


#include "Utils/NativeTypes.h"
#include "ArithmeticOperationInterface.h"

struct DimensionMetaData {
    /*!
     * Constructor.
     * @param N A vector of size=dim and the discrete size of each dimension in
     * each of its entry. For example, a discretization of real space x, y, z
     * with 256x512x64 sites would have N={256,512,64}.
     */
    explicit DimensionMetaData(UIntArray N, RealArray h) : nDim(N.size()), N(N), h(h) {
        assert(N.size() == h.size());
    }

    UInt getNDim()      const                       { return nDim; }
    auto getN(UInt dim) const -> const UInt &       { return N[dim]; }
    auto getN()         const -> const UIntArray &  { return N; }
    auto geth(UInt dim) const -> const Real &       {return h[dim]; }
    auto geth()         const -> const RealArray &  {return h; }

    auto getL(UInt dim) const ->       Real         { return N[dim]*h[dim]; }

    UInt computeFullDiscreteSize() const;

private:
    const UInt nDim;
    UIntArray N;
    RealArray h;
};


class DiscreteSpaceCPU;

class DiscreteSpace : public Utils::ArithmeticOpsInterface<DiscreteSpace> {
public:
    DiscreteSpace(DimensionMetaData dim);
    virtual ~DiscreteSpace();
    auto hostCopy(UInt maxResolution) const -> DiscreteSpaceCPU *;

    const DimensionMetaData& getMetaData() const { return dimensionMetaData; }

    UInt getTotalDiscreteSites() const;

    virtual
    auto dataOnGPU() const -> bool { return false; }
    virtual
    auto getDeviceData()                        const -> const DeviceVector& { throw "trying to access device data on host Space"; };
    virtual
    auto getDeviceData()                              ->       DeviceVector& { throw "trying to access device data on host Space"; };

    auto getHostData(bool sync=false)           const -> const RealArray&;
    auto getHostData(bool sync=false)                 ->       RealArray&;

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


#endif //V_SHAPE_DISCRETESPACE_H

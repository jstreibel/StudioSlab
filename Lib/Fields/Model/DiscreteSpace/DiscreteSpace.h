//
// Created by joao on 11/10/2019.
//

#ifndef V_SHAPE_DISCRETESPACE_H
#define V_SHAPE_DISCRETESPACE_H


#include "Studios/Util/MathTypes.h"
#include "Studios/Util/ArithmeticOpsInterface.h"

struct DimensionMetaData {
    /*!
     * Constructor.
     * @param N A vector of size=dim and the discrete size of each dimension in
     * each of its entry. For example, a discretization of real space x, y, z
     * with 256x512x64 sites would have N={256,512,64}.
     */
    explicit DimensionMetaData(std::vector<PosInt> N) : nDim(N.size()), N(N) {
    }

    PosInt getNDim() const { return nDim; }
    PosInt getN(PosInt dim) const { return N[dim]; }
    const std::vector<PosInt>& getN() const { return N; }
    PosInt computeFullDiscreteSize() const;
    PosInt operator[](PosInt dim) const { return getN(dim); }

private:
    const PosInt nDim;
    std::vector<PosInt> N;
};


class DiscreteSpaceCPU;

class DiscreteSpace : public Utils::ArithmeticOpsInterface<DiscreteSpace> {
public:
    DiscreteSpace(DimensionMetaData dim, Real h);
    [[nodiscard]] DiscreteSpaceCPU *hostCopy(PosInt maxResolution) const;

    [[nodiscard]] PosInt getTotalDiscreteSites() const;
    [[nodiscard]] const VecFloat &getHostData(bool syncWithServer=false) const;
    VecFloat& getHostData(bool syncWithServer=false);

    [[nodiscard]] const DimensionMetaData& getDim() const { return dim; }
    [[nodiscard]] const VecFloat& getX() const;
    VecFloat& getX();
    [[nodiscard]] Real geth() const;

    // TODO colocar essa aqui no operator =
    virtual void setToValue(const DiscreteSpace &param) = 0;

public:
    /*! Sync host with server information. */
    virtual void syncHost() const;

protected:
    const DimensionMetaData dim;
    VecFloat X;
    const Real h;
};


#endif //V_SHAPE_DISCRETESPACE_H

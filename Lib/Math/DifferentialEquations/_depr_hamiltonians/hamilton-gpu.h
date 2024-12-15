#ifndef HamiltonGPU_H
#define HamiltonGPU_H

/*
#include "Core/Base/System.h"
#include "sim/fields/field-gpu.h"
#include <thrust/device_vector.h>

class Hamilton_GPU : public Hamilton
{
public:
    Hamilton_GPU(Field_Cuda *myTempField);
    ~Hamilton_GPU();

    virtual floatt E(const Field &phi);
    virtual floatt Hdens(const Field &phi, const floatt x) const;

    virtual floatt V(const floatt &phi) const;
    virtual floatt dVdphi(const floatt &phi) const;

    virtual Field& dT_F(const Field &phi, Field &out, const floatt dT,
                   const size_t begin, const size_t end);

    static bool setGPU(const unsigned int dev);

private:
    Field_Cuda *temp;
    const size_t N;
};
 */

#endif // HamiltonGPU_H

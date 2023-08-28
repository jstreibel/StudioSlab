#include "hamilton-gpu.h"
#include "sim/fields/field-gpu.h"

#include "sim/math/derivatives-gpu.cuh"
#include "sim/util/device-config.h"

#include <thrust/iterator/constant_iterator.h>
#include <cuda_runtime.h>

extern "C" cudaError_t  cuda_main();

Hamilton_GPU::Hamilton_GPU(Field_Cuda *myTempField) : temp(myTempField), N(myTempField->N) {

}

Hamilton_GPU::~Hamilton_GPU()
{
    delete temp;
}

bool Hamilton_GPU::setGPU(const unsigned int dev){
    int devCount = 0;
    cudaGetDeviceCount(&devCount);
    if(dev > devCount-1) return false;

    cudaError cuErr = cudaSetDevice(dev);

    if(cuErr != cudaSuccess){
        std::cout << "Error setting GPU device: " << cudaGetErrorString(cuErr) << std::endl;
        return false;
    }

    int currDev;
    cuErr = cudaGetDevice(&currDev);
    std::cout << "GPU device set to GPU " << currDev << "." << std::endl;

    return true;
}

floatt Hamilton_GPU::V(const floatt &phi) const { return fabs(phi); }
floatt Hamilton_GPU::dVdphi(const floatt &phi) const { return phi < 0 ? -1 : 1 ;}

struct HamiltonianStepper
{
    const floatt dT;

    HamiltonianStepper(floatt _dT) : dT(_dT) {}

    __host__ __device__
    static inline floatt dVdphi(const floatt &phi){
        // TODO: usar implementacao de sign(x) da GPU.
        return phi < 0 ? -1 : (phi > 0 ? 1 : 0);
    }

    __host__ __device__
    floatt operator ()(const floatt &phi, const floatt &d2Phidx2) const {
        return dT*(-HamiltonianStepper::dVdphi(phi) + d2Phidx2);
    }
};

Field &Hamilton_GPU::dT_F(const Field &phi, Field &out, const floatt dT,
                             const size_t begin, const size_t end)
{
    (void) begin; (void) end;

    const Field_Cuda &inField = static_cast<const Field_Cuda&>(phi);
    Field_Cuda &out1D = static_cast<Field_Cuda&>(out);

    {
        thrust::transform(inField.dPhidT_d.begin(), inField.dPhidT_d.end(),
                          thrust::constant_iterator<floatt>(dT), outField.phi_d.begin(),
                          thrust::multiplies<floatt>());
    }

    {
        DeviceVector &d2dx2_v = temp->phi_d;
        d2dx2(inField.phi_d, d2dx2_v, inField.h, N);
        thrust::transform(inField.phi_d.begin(), inField.phi_d.end(),
                          d2dx2_v.begin(), outField.dPhidT_d.begin(),
                          HamiltonianStepper(dT));
    }

    return outField;
}

floatt Hamilton_GPU::E(const Field &phi) {
    /*
    const Field_CPU &field = static_cast<const Field_Cuda&>(phi);

    const floatt *_phi = &(field.phi[0]);
    const floatt *_dphidt = &(field.dPhidT[0]);

    const size_t N = field.N;

    derivs.setf(_phi);

    //double term1 = -2*V(phi0)*p.L;
    floatt term1 = 1;
    floatt term2 = 0;
    floatt term3 = 0;
    floatt term4 = 0;
    for(size_t i=0; i<N; i++){
        floatt phiX0 = _phi[i];
        const floatt dphidx0 = derivs.dfdx(i);
        const floatt dphidt0 = _dphidt[i];

        term2 += V(phiX0);
        term3 += dphidt0*dphidt0;
        term4 += dphidx0*dphidx0;
    }

    return 0.5*(term1 + derivs.geth()*(2*term2 + term3 + term4));
    */

    return 0;
}

floatt Hamilton_GPU::Hdens(const Field &phi, const floatt x) const
{
    /*
    const Field_CPU &field = static_cast<const Field_CPU&>(phi);

    const Field_CPU::FieldOutType *_phi = &(field.phi[0]);
    const Field_CPU::FieldOutType *_dphidt = &(field.dPhidT[0]);

    derivs.setf(_phi);

    const size_t i = size_t(x/field.L * floatt(field.N));
    floatt phiX0 = _phi[i];
    const floatt dphidx0 = derivs.dfdx(i);
    const floatt dphidt0 = _dphidt[i];

    floatt term1 = -2*V(0);
    floatt term2 = V(phiX0);
    floatt term3 = dphidt0*dphidt0;
    floatt term4 = dphidx0*dphidx0;

    return term1 + term2 + term3 + term4;
    */

    return 0;
}

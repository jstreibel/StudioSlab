//#include "cuda_runtime.h"
#include "cuda.h"
#include "device_launch_parameters.h"

#include "nbody.cuh"
#include "params.cuh"

#include <stdio.h>
#include <stdlib.h>

__constant__ DevFloat R = R_;
__constant__ DevFloat invR = 1.0 / R_;
__constant__ unsigned int N = Nparticles;
__constant__ DevFloat dt = DT;
__constant__ DevFloat dt_2 = 0.5*DT;
__constant__ DevFloat dt2_2 = 0.5*DT*DT;
__constant__ DevFloat boxSide = 2.0 * BOX_HALF_SIDE;
__constant__ DevFloat boxHalfSide = BOX_HALF_SIDE;

#if 1
#define CHECK(cudaStatus, msg) if(cudaStatus != 0) {printf("cudaError %i: %s - %s\n", cudaStatus, cudaGetErrorString(cudaStatus), msg);}
#else
#define CHECK(cudaStatus, msg)
#endif


#ifndef __syncthreads
#define __syncthreads()
#endif
#ifndef sqrtf
#define sqrtf(a) a
#endif

const size_t SIZE = Nparticles * sizeof(Real2);
Real2 *dev_r = NULL;
Real2 *dev_p = NULL;
Real2 *dev_a = NULL;

const char *getIntMethodStr()
{
#if INT_METHOD == EULER
	return "Euler";
#elif INT_METHOD == LEAPFROG
	return "Leapfrog";
#else
	return "?";
#endif
}

/* BODY BODY */
__device__ Real2 bodyBodyInteraction(Real2 pi, Real2 pj, Real2 ai)
{
	Real2 r;

	// 2 FLOPS
	r.x = pj.x - pi.x;
	r.y = pj.y - pi.y;

	// 4 FLOPS
	Real distSqr = r.x*r.x + r.y*r.y + EPS;

	// 3 FLOPS (1 sqrt, 1 inv)
	Real dist = Sqrt(distSqr);
	Real invDist = { 1.0 / (dist) };

	// 
	if (dist < R)
	{
		const DevFloat arg = 1.0 - dist*invR;
		const DevFloat argsqr = arg*arg;
		Real invr_f = invDist * argsqr*argsqr;
		ai.x -= r.x*invr_f;
		ai.y -= r.y*invr_f;
	}

	/*
	Real invDistCube = 1.0f / (dist*distSqr);
	ai.x += r.x * invDistCube;
	ai.y += r.y * invDistCube;
	*/

	return ai;
}

__global__ void calculateForces(Real2 *dev_r, Real2 *dev_accel);
__global__ void applyForces(Real2 *dev_r, Real2 *dev_p, Real2 *dev_a);

__global__ void memSet(Real2 *devPtr, DevFloat val)
{
	int idx = blockIdx.x*blockDim.x + threadIdx.x;
	Real2 *globalPtr = (Real2*)devPtr;

	Real2 local = globalPtr[idx];

	local.x = val;
	local.y = val;

	globalPtr[idx] = local;
}

unsigned int getN() { return Nparticles; }
Real getR() { return R_; }
Real getBoxHalfSide() { return BOX_HALF_SIDE; }

__host__ void initDevice(Real2 *host_r, Real2 *host_p, 
						 Real2 *dev_r,  Real2 *dev_p, Real2 *dev_a,
						 const size_t N, const size_t P_)
{
	if (Nparticles%P_ || ((Nparticles / P_)>1024)) {
		printf("\nTidak bagus.\n");
		exit(0);
	}

	cudaError_t cudaStatus;

	/*
	cudaStatus = cudaMemcpyToSymbol(dtDev, &dt, 4, 0, cudaMemcpyKind::cudaMemcpyHostToDevice);
	CHECK(cudaStatus, "in konstant memcpy.");
	cudaStatus = cudaMemcpyToSymbol(NDev, &N, 4, 0, cudaMemcpyKind::cudaMemcpyHostToDevice);
	CHECK(cudaStatus, "in konstant memcpy.");
	*/

	cudaStatus = cudaSetDevice(1);
	CHECK(cudaStatus, "while setting device.");

	cudaStatus = cudaMalloc((void**)&dev_r, SIZE);
	CHECK(cudaStatus, "on alloc dev_r.");
	cudaStatus = cudaMalloc((void**)&dev_p, SIZE);
	CHECK(cudaStatus, "on alloc dev_p.");
	cudaStatus = cudaMalloc((void**)&dev_a, SIZE);
	CHECK(cudaStatus, "on alloc dev_a.");

	cudaStatus = cudaMemcpy(dev_r, host_r, SIZE, cudaMemcpyHostToDevice);
	CHECK(cudaStatus, "on memcpy host_r --> dev_r.");
	cudaStatus = cudaMemcpy(dev_p, host_p, SIZE, cudaMemcpyHostToDevice);
	CHECK(cudaStatus, "on memcpy host_p --> dev_p.");

	memSet <<<Nparticles / P_, P_ >>>(dev_a, 0.0);
	cudaError_t cudaError = cudaGetLastError();
	if (cudaError) CHECK(cudaError, "on damping speed.");

	CHECK(cudaDeviceSynchronize(), " on device sync after init memcopies.");
}

__host__ void initDevice(Real2 *host_r, Real2 *host_p)
{
	if (Nparticles%P || ((Nparticles/P)>1024)) {
		printf("\nTidak bagus.\n");
		exit(0);
	}

	cudaError_t cudaStatus;

	/*
	cudaStatus = cudaMemcpyToSymbol(dtDev, &dt, 4, 0, cudaMemcpyKind::cudaMemcpyHostToDevice);
	CHECK(cudaStatus, "in konstant memcpy.");
	cudaStatus = cudaMemcpyToSymbol(NDev, &N, 4, 0, cudaMemcpyKind::cudaMemcpyHostToDevice);
	CHECK(cudaStatus, "in konstant memcpy.");
	*/

	cudaStatus = cudaSetDevice(1);
	CHECK(cudaStatus, "while setting device.");

	cudaStatus = cudaMalloc((void**)&dev_r, SIZE);
	CHECK(cudaStatus, "on alloc dev_r.");
	cudaStatus = cudaMalloc((void**)&dev_p, SIZE);
	CHECK(cudaStatus, "on alloc dev_p.");
	cudaStatus = cudaMalloc((void**)&dev_a, SIZE);
	CHECK(cudaStatus, "on alloc dev_a.");

	cudaStatus = cudaMemcpy(dev_r, host_r, SIZE, cudaMemcpyHostToDevice);
	CHECK(cudaStatus, "on memcpy host_r --> dev_r.");
	cudaStatus = cudaMemcpy(dev_p, host_p, SIZE, cudaMemcpyHostToDevice);
	CHECK(cudaStatus, "on memcpy host_p --> dev_p.");

	memSet<<<Nparticles/P, P>>>(dev_a, 0.0);
	cudaError_t cudaError = cudaGetLastError();
	if (cudaError) CHECK(cudaError, "on damping speed.");

	CHECK(cudaDeviceSynchronize(), " on device sync after init memcopies.");
}

__host__ Real2 *fetch_rFromDevice(Real2 *rHost)
{
	cudaError_t cudaStatus;
	cudaStatus = cudaMemcpy(rHost, dev_r, Nparticles * sizeof(Real2), cudaMemcpyDeviceToHost);
	CHECK(cudaStatus, "on fetch r from device.");

	return rHost;

}
__host__ Real2 *fetch_pFromDevice(Real2 *pHost)
{
	cudaError_t cudaStatus;
	cudaStatus = cudaMemcpy(pHost, dev_p, Nparticles * sizeof(Real2), cudaMemcpyDeviceToHost);
	CHECK(cudaStatus, "on fetch p from device.");

	return pHost;
}

__global__ void dampSpeed_k(Real2 *dev_p, DevFloat factor)
{
	int idx = blockIdx.x*blockDim.x + threadIdx.x;
	Real2 *pGlobal = (Real2*)dev_p;

	Real2 pLocal = pGlobal[idx];

	pLocal.x *= factor;
	pLocal.y *= factor;

	pGlobal[idx] = pLocal;
}
__host__ void dampSpeed(DevFloat factor)
{
	const int nBlocks = Nparticles / P;
	const int nThreadsPerBlock = P;

	dampSpeed_k <<<nBlocks, nThreadsPerBlock >>> (dev_p, factor);
	cudaError_t cudaError = cudaGetLastError();
	if (cudaError) CHECK(cudaError, "on damping speed.");
	CHECK(cudaDeviceSynchronize(), " on device sync after speed damp.");
}

void step()
{
	const int nBlocks = Nparticles / P;
	const int nThreadsPerBlock = P;

	cudaError_t cudaError;

#if INT_METHOD == LEAPFROG
	cudaError = cudaMemcpy(dev_a0, dev_a1, SIZE, cudaMemcpyDeviceToDevice);
	CHECK(cudaError, "on cudaMemcpy(dev_a0, dev_a1, ...)");
#endif

	calculateForces <<<nBlocks, nThreadsPerBlock, P*sizeof(Real2)>>> (dev_r, dev_a);
	cudaError = cudaGetLastError();
	if (cudaError) CHECK(cudaError, "on launching calculateForces.");
	CHECK(cudaDeviceSynchronize(), " on device sync after calculateForces.");
	
	applyForces <<<nBlocks, nThreadsPerBlock>>> (dev_r, dev_p, dev_a);
	cudaError = cudaGetLastError();
	if(cudaError) CHECK(cudaError, "on launching applyForces.");
	CHECK(cudaDeviceSynchronize(), " on device sync after applyForces.");
}

__device__ Real2 bodyBoxInteraction(Real2 r, Real2 a) {
	// Collide with box:
	const DevFloat k = 1000.0;
	if      (r.x >  boxHalfSide) a.x += -k*(1.0f - boxHalfSide / r.x);
	else if (r.x < -boxHalfSide) a.x +=  k*(1.0f + boxHalfSide / r.x);
	
	if      (r.y >  boxHalfSide) a.y += -k*(1.0f - boxHalfSide / r.y);
	else if (r.y < -boxHalfSide) a.y +=  k*(1.0f + boxHalfSide / r.y);

	return a;
}

__device__ Real2 externalField(Real2 r, Real2 a) {
	a.y -= 5.e-1;
	return a;
}


__device__ Real2 tileCalculation(Real2 myPosition, Real2 accel)
{
	int i;
	extern __shared__ Real2 rShared[];

	for (i = 0; i < blockDim.x; i++)
	{
		Real2 p2 = rShared[i];
		accel = bodyBodyInteraction(myPosition, p2, accel);
#if BOUNDARY == PERIODIC
		p2.y += BOX_SIDE;
		accel = bodyBodyInteraction(myPosition, p2, accel);
		p2.x += BOX_SIDE;
		accel = bodyBodyInteraction(myPosition, p2, accel);
		p2.y -= BOX_SIDE;
		accel = bodyBodyInteraction(myPosition, p2, accel);
		p2.y -= BOX_SIDE;
		accel = bodyBodyInteraction(myPosition, p2, accel);
		p2.x -= BOX_SIDE;
		accel = bodyBodyInteraction(myPosition, p2, accel);
		p2.x -= BOX_SIDE;
		accel = bodyBodyInteraction(myPosition, p2, accel);
		p2.y += BOX_SIDE;
		accel = bodyBodyInteraction(myPosition, p2, accel);
		p2.y += BOX_SIDE;
		accel = bodyBodyInteraction(myPosition, p2, accel);		
#endif

	}

	return accel;
}

__global__ void calculateForces(Real2 *dev_r, Real2 *dev_accel)
{
	extern __shared__ Real2 rShared[];

	Real2 *rGlobal = (Real2 *)dev_r;
	Real2 *accelGlobal = (Real2 *)dev_accel;
	Real2 rLocal;
	int i, tile;
	Real2 accel = {0.0f, 0.0f};
	int gtid = blockIdx.x * blockDim.x + threadIdx.x;

	rLocal = rGlobal[gtid];

#if BOUNDARY == RIGID
	accel = bodyBoxInteraction(rLocal, accel);
#endif
#if EXTERNAL_FIELD
	accel = externalField(rLocal, accel);
#endif
	for (i = 0, tile = 0; i < N; i += P, tile++)
	{
		int idx = tile * blockDim.x + threadIdx.x;
		rShared[threadIdx.x] = rGlobal[idx];
		__syncthreads();
		accel = tileCalculation(rLocal, accel);
		__syncthreads();
	}
	// save result in global memory for the integration step.
	Real2 accel2 = {accel.x, accel.y};
	accelGlobal[gtid] = accel2;
}

__global__ void applyForces(Real2 *dev_r, Real2 *dev_p, Real2 *dev_a)
{
	int idx = blockIdx.x*blockDim.x + threadIdx.x;
	Real2 *rGlobal = (Real2*)dev_r;
	Real2 *pGlobal = (Real2*)dev_p;
	Real2 *aGlobal = (Real2*)dev_a;

	Real2 rLocal = rGlobal[idx];
	Real2 pLocal = pGlobal[idx];
	Real2 aLocal = aGlobal[idx];

	pLocal.x += (aLocal.x)*dt_2;
	pLocal.y += (aLocal.y)*dt_2;

	rLocal.x += pLocal.x*dt;
	rLocal.y += pLocal.y*dt;

#if BOUNDARY == PERIODIC
	if (rLocal.x >  boxHalfSide) rLocal.x -= boxSide;
	else if (rLocal.x < -boxHalfSide) rLocal.x += boxSide;
	if (rLocal.y >  boxHalfSide) rLocal.y -= boxSide;
	else if (rLocal.y < -boxHalfSide) rLocal.y += boxSide;
#endif

	rGlobal[idx] = rLocal;
	pGlobal[idx] = pLocal;
}

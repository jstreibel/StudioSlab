/*
 *  APIAdapter.h
 *  SSMFramework
 *
 *  Created by Joao Streibel on 07/12/2010.
 *  No rights reserved.
 *
 */

#ifndef SSM_API
#define SSM_API

#include <CUDA/CUDA.h>

class APIAdapter
{
public:
	void setOccluderHandle(int handle) {occluderMapHandle = handle;}
	void setReceiverHandle(int handle) {receiverMapHandle = handle;}
	void setSSMHandle(int handle) {SSMMapHandle = handle;}
	
	virtual float *getSSMDevicePointer() = 0;
	virtual float *getSSMHostPointer() = 0;

	virtual float *getOccludersDevicePointer() = 0;
	virtual float *getOccludersHostPointer() = 0;

	virtual float *getReceiversDevicePointer() = 0;
	virtual float *getReceiversHostPointer() = 0;

	virtual void finish() = 0;

protected:
	int occluderMapHandle;
	int receiverMapHandle;
	int SSMMapHandle;
};

#endif
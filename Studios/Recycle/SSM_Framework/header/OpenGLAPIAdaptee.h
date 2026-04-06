/*
 *  OpenGLAPIAdapter.h
 *  SSMFramework
 *
 *  Created by Joao Streibel on 13/12/2010.
 *  No rights reserved.
 *
 */

#include "APIAdapter.h"

class OpenGLAPIAdaptee : public APIAdapter
{
public:
	float *getSSMDevicePointer()
	{
		return 0;
	}
	float *getSSMHostPointer()
	{
		return 0;
	}
	
	float *getOccludersDevicePointer()
	{
		return 0;
	}
	float *getOccludersHostPointer()
	{
		return 0;
	}
	
	float *getReceiversDevicePointer()
	{
		return 0;	
	}
	float *getReceiversHostPointer()
	{
		return 0;
	}
	
	void finish()
	{
	
	}
};
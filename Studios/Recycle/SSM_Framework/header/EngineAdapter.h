/*
 *  EngineAdapter.h
 *  SSMFramework
 *
 *  Created by Joao Streibel on 07/12/2010.
 *  No rights reserved.
 *
 */

#ifndef SSM_ENGINE
#define SSM_ENGINE

#include "Light.h"

class EngineAdapter
{
public:
	EngineAdapter(void *occluderData = 0, void *receiverData = 0,
				  Light *lightCamera = 0)
	{
		this->occluderData = occluderData;
		this->receiverData = receiverData;
		this->lightCamera = lightCamera;		
	}

	void setOccluderData(void *data) {this->occluderData = data; }
	void setReceiverData(void *data) {this->receiverData = data; }
	void setLight(Light *camera) {this->lightCamera = camera; }

	// Retrieve receivers depth map image handle
	virtual unsigned int retrieveReceiverDepthMapHandle() = 0;
	// Retrieve occluders depth map image handle
	virtual unsigned int retrieveOccluderDepthMapHandle() = 0;

	// Get texture handle on which to draw SSM
	virtual unsigned int getSSMLuminanceTextureHandle() = 0;
	
	virtual void computeDepthMaps(int width, int height) = 0;

	// Finalize!
	virtual void *setupSSMProjection() = 0;

	// Just in case..
	virtual void *retrieveSSMTexture() = 0;

	virtual void finish() = 0;

protected:
	void *occluderData;
	void *receiverData;
	Light *lightCamera;
};

#endif
/*
 *  SSMGenerationMediator.cpp
 *  SSMFramework
 *
 *  Created by Joao Streibel on 08/12/2010.
 *  No rights reserved.
 *
 */

#include "../header/SSMGenerationMediator.h"

void SSMGenerationMediator::generateSSM()
{
	engine->computeDepthMaps(mapWidth, mapHeight);
	unsigned int occluderHandle = engine->retrieveOccluderDepthMapHandle();
	unsigned receiverHandle = engine->retrieveReceiverDepthMapHandle();
	unsigned SSMHandle = engine->getSSMLuminanceTextureHandle();
	
	API->setOccluderHandle(occluderHandle);
	API->setReceiverHandle(receiverHandle);
	API->setSSMHandle(SSMHandle);
	
	float *occluderPixels = NULL;
	float *receiverPixels = NULL;
	float *SSMPixels = NULL;
	if (strategy->getLocation() == SSMGenerationStrategy::DEVICE){
		occluderPixels = API->getOccludersDevicePointer();
		receiverPixels = API->getReceiversDevicePointer();
		SSMPixels = API->getSSMDevicePointer();			
	}
	else if(strategy->getLocation() == SSMGenerationStrategy::HOST){
		occluderPixels = API->getOccludersHostPointer();
		receiverPixels = API->getReceiversHostPointer();
		SSMPixels = API->getSSMHostPointer();
	}
	
	strategy->setOccludersPointer(occluderPixels);
	strategy->setReceiversPointer(receiverPixels);
	strategy->setSSMPointer(SSMPixels);
	strategy->computeSSM();
	
	API->finish();
	
	engine->finish();
};
/*
 *  SSMGeneratorFactory.cpp
 *  SSMFramework
 *
 *  Created by Joao Streibel on 08/12/2010.
 *  No rights reserved.
 *
 */

#include "../header/SSMGeneratorFactory.h"

// Engine adaptee(s) #include's:
#include "../header/OpenGLTestEngineAdaptee.h"
#include "../header/OpenGLAPIAdaptee.h"
#include "../header/CPUStrategy.h"

// API adaptee(s) #include's:

// Strategy's #include's:


SSMGeneratorFactory *SSMGeneratorFactory::singleton = 0;

bool SSMGeneratorFactory::init(ENGINE engine, GRAPHIC_API graphicAPI){
	if (SSMGeneratorFactory::singleton == 0){
		SSMGeneratorFactory::singleton = new SSMGeneratorFactory(engine, graphicAPI);
		return true;
	}
	return false;
};

SSMGenerationMediator *SSMGeneratorFactory::createSSMMediator(){
	SSMGenerationMediator *mediator = new SSMGenerationMediator(64, 64);
	OpenGLTestEngineAdaptee *engine = new OpenGLTestEngineAdaptee();
	mediator->setEngine(engine);
	
	APIAdapter *api = new OpenGLAPIAdaptee();
	mediator->setAPI(api);
	api->setSSMHandle(engine->getSSMLuminanceTextureHandle());
	api->setOccluderHandle(engine->retrieveOccluderDepthMapHandle());
	api->setReceiverHandle(engine->retrieveReceiverDepthMapHandle());
	
	CPUStrategy *strategy = new CPUStrategy();
	mediator->setSSMGenerationStrategy(strategy);
	
	return mediator;
};

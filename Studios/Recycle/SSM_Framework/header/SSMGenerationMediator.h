/*
 *  SSMGenerationMediator.h
 *  SSMFramework
 *
 *  Created by Joao Streibel on 07/12/2010.
 *  No rights reserved.
 *
 */

#ifndef SSM_MEDIATOR
#define SSM_MEDIATOR

#include "EngineAdapter.h"
#include "APIAdapter.h"
#include "SSMGenerationStrategy.h"

#include "Light.h"

class SSMGenerationMediator{
public:
	SSMGenerationMediator(int w, int h)	{mapWidth = w; mapHeight = h;}
	
	void setEngine(EngineAdapter *engine) {this->engine = engine;}
	void setAPI(APIAdapter *API) {this->API = API;}
	void setSSMGenerationStrategy(SSMGenerationStrategy *strategy) {this->strategy = strategy;}
	
	void setLight(Light *light) {this->light = light;}
	
	void setMapDimension(int w, int h) {mapWidth = w; mapHeight = h;}
	
	void generateSSM();
	
	void *getSSMTexture() {return engine->retrieveSSMTexture();	}
	
private:
	EngineAdapter *engine;
	APIAdapter *API;
	SSMGenerationStrategy *strategy;
	
	int mapWidth, mapHeight;
		
	Light *light;
};

#endif
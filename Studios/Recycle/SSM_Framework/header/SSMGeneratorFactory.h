/*
 *  SSMGeneratorFactory.h
 *  SSMFramework
 *
 *  Created by Joao Streibel on 08/12/2010.
 *  No rights reserved.
 *
 */

#ifndef SSM_FACTORY
#define SSM_FACTORY

#include "SSMGenerationMediator.h"

class SSMGeneratorFactory
{
public:
	typedef enum ENGINE {OPENGL_TEST_ENGINE, } ENGINE;
	typedef enum GRAPHIC_API {OPEN_GL, } GRAPHIC_API;
	
private:
	SSMGeneratorFactory(ENGINE engine, GRAPHIC_API graphicAPI){ 
		this->engine = engine;
		this->graphicAPI = graphicAPI;
	}
		
public:
	static bool init(ENGINE engine, GRAPHIC_API graphicAPI);
	static SSMGeneratorFactory *getSingleton() {return singleton; }
	
public:
	SSMGenerationMediator *createSSMMediator();
	
private:	
	ENGINE engine;
	GRAPHIC_API graphicAPI;
	
	static SSMGeneratorFactory *singleton;
};

#endif

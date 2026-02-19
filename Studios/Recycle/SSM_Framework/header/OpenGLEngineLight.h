/*
 *  OpenGLEngineLight.h
 *  SSMFramework
 *
 *  Created by Joao Streibel on 13/12/2010.
 *  No rights reserved.
 *
 */

#ifndef OPENGL_LIGHT
#define OPENGL_LIGHT

#include "OpenGLEngineCamera.h"

#include "Light.h"

class OpenGLEngineLight : public Light{
public:
	OpenGLEngineLight(OpenGLEngineCamera *lightCamera) : Light(lightCamera)
	{
		camera = lightCamera;
		
	}
	
	void setFOVy(float FOVy){camera->fovY = FOVy;}
	void setZFar(float zFar){camera->zFar = zFar;}
	void setZNear(float zNear){camera->zNear = zNear;}
	
	float getFOVy(){return camera->fovY;}
	float getZFar(){return camera->zFar;}
	float getZNear(){return camera->zNear;}
	
	float getPosX(){return camera->posX;}
	float getPosY(){return camera->posY;}
	float getPosZ(){return camera->posZ;}
	
private:
	OpenGLEngineCamera *camera;
};

#endif
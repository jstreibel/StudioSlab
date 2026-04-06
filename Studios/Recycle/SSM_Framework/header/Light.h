/*
 *  Light.h
 *  SSMFramework
 *
 *  Created by Joao Streibel on 07/12/2010.
 *  No rights reserved.
 *
 */

#ifndef SSM_LIGHT
#define SSM_LIGHT

class Light 
{
public:
	Light(void *lightCamera) {this->lightCamera = lightCamera;}
	void setLightCamera(void *light) {this->lightCamera = light;}
	void setDimensions(float width, float height){
		this->width = width;
		this->height = height;
	}
	
	virtual void setFOVy(float FOVy) = 0;
	virtual void setZFar(float zFar) = 0;
	virtual void setZNear(float zNear) = 0;
	
	virtual float getFOVy() = 0;
	virtual float getZFar() = 0;
	virtual float getZNear() = 0;
	
	virtual float getPosX() = 0;
	virtual float getPosY() = 0;
	virtual float getPosZ() = 0;
	
	float getWidth() {return width;}
	float getHeight() {return height;}
	
protected:
	void *lightCamera;
	float width, height;
};

#endif
/*
 *  untitled.cpp
 *  SSMFramework
 *
 *  Created by Joao Streibel on 09/12/2010.
 *  No rights reserved.
 *
 */

#include "../header/OpenGLTestEngineAdaptee.h"

#include "../header/MeshStructs.h"

#include "../header/OtherFuncs.h"
#include "../header/MeshFuncs.h"

#include <vector>
typedef std::vector<Mesh*> MeshVector;

OpenGLTestEngineAdaptee::OpenGLTestEngineAdaptee(){
	//glGenTextures(1, &this->occluderHandle);
	//glGenTextures(1, &this->receiverHandle);
	//glGenTextures(1, &this->SSMHandle);
	this->occluderHandle = 1;
	this->receiverHandle = 2;
	this->SSMHandle = 3;
}

void OpenGLTestEngineAdaptee::setupLightCamera()
{
	// USE COLOR BUFFER FOR LIGHT PASS, SET VIEWPORT ACCORDINGLY
	glViewport(0, 0, 64, 64);
	
	// CALC LIGHT'S VIEW NEAR & FAR PLANE AND Y FOV
	const float jeepRadius = 10.0f*1.5f;
	const float lightToJeepDistance = Distance(Vertex(lightCamera->getPosX(),
													  lightCamera->getPosY(),
													  lightCamera->getPosZ()), 
											   Vertex(.0f,.0f,.0f));
	const float nearPlaneCandidate = lightToJeepDistance - jeepRadius;
	
	const float nearPlane = nearPlaneCandidate > 0.1f ? nearPlaneCandidate : 0.1f;
	//const float farPlane = nearPlane + (2.0f*jeepRadius);
	const float farPlane = 
	nearPlaneCandidate > 0.1f ? 
	((nearPlane + (2.0f*jeepRadius))) : 
	((2.0f*jeepRadius));
	const float fovY = RadToDeg_Float(2.0f * atanf(jeepRadius / lightToJeepDistance));
	
	// SET CAMERA TO LIGHT VIEW
	{
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(fovY, 1.0f, nearPlane, farPlane);
		
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(lightCamera->getPosX(), lightCamera->getPosY(), 
				  lightCamera->getPosZ(), 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	}
	
	// CALC TEXTURE MATRIX FOR SHADOW MAP PROJECTION
	{
		float lightProjection[16], lightModelview[16], tempMatrix[16];
		glGetFloatv(GL_PROJECTION_MATRIX, lightProjection);
		glGetFloatv(GL_MODELVIEW_MATRIX, lightModelview);
		
		m3dLoadIdentity44(tempMatrix);
		m3dTranslateMatrix44(tempMatrix, 0.5f, 0.5f, 0.5f);
		m3dScaleMatrix44(tempMatrix, 0.5f, 0.5f, 0.5f);
		m3dMatrixMultiply44(textureMatrix, tempMatrix, lightProjection);
		m3dMatrixMultiply44(tempMatrix, textureMatrix, lightModelview);
		m3dTransposeMatrix44(textureMatrix, tempMatrix);
	}
	
	// DISABLE UNNECESSARY STATES
	{
		glShadeModel(GL_FLAT);
		glDisable(GL_LIGHTING);
		glDisable(GL_COLOR_MATERIAL);
		glDisable(GL_NORMALIZE);
		glColorMask(0, 0, 0, 0);
	}
	
	glEnable(GL_DEPTH_TEST);
}


GLuint OpenGLTestEngineAdaptee::retrieveReceiverDepthMapHandle()
{	
	return receiverHandle;
}

GLuint OpenGLTestEngineAdaptee::retrieveOccluderDepthMapHandle()
{	
	return occluderHandle;
}

void OpenGLTestEngineAdaptee::computeDepthMaps(int width, int height)
{	
	MeshVector *receivers = (MeshVector*)receiverData;
	setupLightCamera();
	
	glDisable(GL_POLYGON_OFFSET_FILL);
	for (int i=0; i<receivers->size(); ++i) {
		DrawMesh((*receivers)[i]);
	}
	
	
	glReadPixels(0, 0, 64, 64,
				 GL_DEPTH_COMPONENT, GL_FLOAT, (void*)receiverPixels);
	
	glBindTexture(GL_TEXTURE_2D, receiverHandle);
	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
					 0, 0, 64, 64, 0);
	
	
	
	MeshVector *occluders = (MeshVector*)occluderData;
	setupLightCamera();
	
	glDisable(GL_POLYGON_OFFSET_FILL);
	for (int i=0; i<occluders->size(); ++i) {
		DrawMesh((*occluders)[i]);
	}
	
	
	glReadPixels(0, 0, 64, 64,
				 GL_DEPTH_COMPONENT, GL_FLOAT, (void*)occluderPixels);
	
	glBindTexture(GL_TEXTURE_2D, occluderHandle);
	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
					 0, 0, 64, 64, 0);
}

GLuint OpenGLTestEngineAdaptee::getSSMLuminanceTextureHandle()
{
	return SSMHandle;
}

void *OpenGLTestEngineAdaptee::setupSSMProjection()
{
	return 0;
}

void *OpenGLTestEngineAdaptee::retrieveSSMTexture()
{
	return 0;
}

void OpenGLTestEngineAdaptee::finish()
{
}
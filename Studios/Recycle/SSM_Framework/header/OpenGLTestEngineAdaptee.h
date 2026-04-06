/*
 *  SSMOpenGLTestEngineAdaptee.h
 *  SSMFramework
 *
 *  Created by Joao Streibel on 08/12/2010.
 *  No rights reserved.
 *
 */

#ifndef SSM_OPENGL_TESTENGINE_ADAPTEE
#define SSM_OPENGL_TESTENGINE_ADAPTEE

#include "EngineAdapter.h"

#include <GLUT/GLUT.h>

class OpenGLTestEngineAdaptee : public EngineAdapter {
public:	
	OpenGLTestEngineAdaptee();
	
	GLuint retrieveReceiverDepthMapHandle();
	GLuint retrieveOccluderDepthMapHandle();
	
	GLuint getSSMLuminanceTextureHandle();
	
	void computeDepthMaps(int width, int height);
	
	void *setupSSMProjection();
	
	void *retrieveSSMTexture();
	void finish();
	
private:
	void setupLightCamera();
	
private:	
	GLuint occluderHandle;
	GLuint receiverHandle;
	GLuint SSMHandle;
	
	float receiverPixels[64*64];
	float occluderPixels[64*64];
	
	float textureMatrix[16];
};

#endif
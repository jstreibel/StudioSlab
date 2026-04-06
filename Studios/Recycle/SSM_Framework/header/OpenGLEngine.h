/*
 *  OpenGLEngine.h
 *  SSMFramework
 *
 *  Created by Joao Streibel on 10/12/2010.
 *  No rights reserved.
 *
 */

#ifndef OPENGL_ENGINE
#define OPENGL_ENGINE

#include "Structs.h"
#include "MeshStructs.h"
#include "OBJ_Reader.h"
#include "MeshFuncs.h"
//#include "VisualDebugging.h"
#include "OtherFuncs.h"

#include <GLUT/GLUT.h>
#include <GLUI/GLUI.h>

#define GL_ERROR(val)														\
	{																		\
		if(val != 0) std::cout << "\nOpenGL error check " << val << ". ";	\
		GLenum code;														\
		while ((code = glGetError())!=GL_NO_ERROR)							\
			std::cout << (char*)gluErrorString(code) << ". ";				\
	}

#define MODE__DRAW_SHADOWED_SCENE 0
#define MODE__SHOW_PATCHES_VIEW 1
const int numModes = 2;
int currentMode = 0;
bool rebuildShadowMap = true;
int drawJeepPatches = 0;

class OpenGLEngine
{
public:
	OpenGLEngine(){
		
		glEnable(GL_DEPTH_TEST);
		
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		
		backColor[0] = 0.607843137254902f;
		backColor[1] = 0.7686274509803922f;
		backColor[2] = 1.0f;
		backColor[3] = 1.0f;
		glClearColor(backColor[0], backColor[1], backColor[2], backColor[3]);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glGenTextures(1, &softShadowTexID);
		glBindTexture(GL_TEXTURE_2D, softShadowTexID);
		GL_ERROR(1);
		float *pixels = new float[64*64*4];
		for(int i=0; i<64*64; ++i)
		{
			if(0)
			{
				pixels[i*4 + 0] = 1.0f;
				pixels[i*4 + 1] = 0.0f;
				pixels[i*4 + 2] = 1.0f;
				pixels[i*4 + 3] = 1.0f;
			}
			else
				pixels[i] = i%2==0 ? 1.0f : 0.0f;
		}
		
		glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE32F_ARB, 64, 64, 0, GL_LUMINANCE, GL_FLOAT, pixels);
		//copyLuminanceTo2DTexture(NULL, softShadowTextureID, 64, 64);
		GL_ERROR(2);
		
		GLfloat camMatrix[16] = 
		{
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};
		
		for (int i=0; i<16; ++i)
			cameraMatrix[i] = camMatrix[i];
		
		GLfloat lPos[]	= {2.0f, 5.0f, 1.0f, 1.0f};
		for (int i=0; i<4; ++i) 
			lightPos[i] = lPos[i];
		
		textureFilter = GL_LINEAR;
		
		occluderMapWidth = 64;
		occluderMapHeight = 64;
		
		lightFOVExageration = 1.0f;
		farPlaneOffsetFactor = 2.0f;
		
		drawNearAndFarPlanes = 0;
		drawOccluderPatches = 1;
		drawPenumbraExtents = 0;
		
		cameraDistance = 10.0f;
		camFovY = 60.0f;
		
		InitScene();
	}
	
	void InitScene()
	{
		lightState.lightArea.depth = 3.0f;
		lightState.lightArea.width = 3.0f;
		
		lightState.occluderDepthMapPixels = 0;
		lightState.receiverDepthMapPixels = 0;
				
		jeep = new Mesh;
		ground = new Mesh;
		light = new Mesh;
		
		OBJ_Reader::load(jeep, (char*)"/Users/jstreibel/Documents/PROG/SoftShadows/Resource/Jeep.obj");
		jeepPos[0] = 0.0f;
		jeepPos[1] = -1.0f;
		jeepPos[2] = 0.0f;
		//OBJ_Reader::load(jeep, "Resource/Cylinder.obj");
		
		OBJ_Reader::load(ground, (char*)"/Users/jstreibel/Documents/PROG/SoftShadows/Resource/Ground.obj");
		//OBJ_Reader::load(ground, "Resource/Ground02.obj");
		
		OBJ_Reader::load(light, (char*)"/Users/jstreibel/Documents/PROG/SoftShadows/Resource/Light.obj");
		
		jeepAABB = new AABB(FindMeshAABB(jeep));	
	}
	
	void DrawScene()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		CameraFullPass();
		
		glutSwapBuffers();
	}
	
	LightState LightDepthPass_()
	{
		// USE COLOR BUFFER FOR LIGHT PASS, SET VIEWPORT ACCORDINGLY
		glViewport(0, 0, occluderMapWidth, occluderMapHeight);
		
		// CALC LIGHT'S VIEW NEAR & FAR PLANE AND Y FOV
		const float jeepRadius = RetrieveRadius(*jeepAABB)*1.5f;
		const float lightToJeepDistance = Distance(Vertex(lightPos), Vertex(jeepPos));
		const float nearPlaneCandidate = lightToJeepDistance - jeepRadius;
		
		const float nearPlane = nearPlaneCandidate > 0.1f ? nearPlaneCandidate : 0.1f;
		//const float farPlane = nearPlane + (2.0f*jeepRadius);
		const float farPlane = 
		nearPlaneCandidate > 0.1f ? 
		((nearPlane + (2.0f*jeepRadius))*farPlaneOffsetFactor) : 
		((2.0f*jeepRadius)*farPlaneOffsetFactor);
		const float fovY = RadToDeg_Float(2.0f * atanf(jeepRadius / lightToJeepDistance))*lightFOVExageration;
		
		// UPDATE LIGHT AND MAPS STATE 
		{
			lightState.occluderDepthTexID = occluderTexID;
			lightState.mapWidth  = occluderMapWidth;
			lightState.mapHeight = occluderMapHeight;
			lightState.zNear = nearPlane;
			lightState.zFar = farPlane;
			lightState.fovY = fovY;
			
			lightState.lightArea.pos.x = 0.0f;
			lightState.lightArea.pos.y = 0.0f;
			lightState.lightArea.    z = 0.0f;
			
			/*if(occluderMapResized)
			 {
			 delete [] lightState.occluderDepthMapPixels;
			 lightState.occluderDepthMapPixels = 0;
			 
			 delete [] lightState.receiverDepthMapPixels;
			 lightState.receiverDepthMapPixels = 0;
			 }*/
			
			//const int totalSize = lightState.mapWidth*lightState.mapHeight;
			
			if(lightState.occluderDepthMapPixels == NULL)
				lightState.occluderDepthMapPixels = new GLfloat[512*512];
			
			if(lightState.receiverDepthMapPixels == NULL)
				lightState.receiverDepthMapPixels = new GLfloat[512*512];
		}
		
		// SET CAMERA TO LIGHT VIEW
		{
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			gluPerspective(fovY, 1.0f, nearPlane, farPlane);
			
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			gluLookAt(lightPos[0], lightPos[1], lightPos[2], 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
		}
		
		// CALC TEXTURE MATRIX FOR SHADOW MAP PROJECTION
		{
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
		
		// RECEIVERS PASS
		{
			glDisable(GL_POLYGON_OFFSET_FILL);
			DrawGround();
			
			glReadPixels(0, 0, occluderMapWidth, occluderMapHeight,
						 GL_DEPTH_COMPONENT, GL_FLOAT, lightState.receiverDepthMapPixels);
			
			glBindTexture(GL_TEXTURE_2D, receiverTexID);
			glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
							 0, 0,
							 occluderMapWidth, occluderMapHeight, 0);
			
			lightState.receiverDepthTexID = receiverTexID;
		}
		
		// OCCLUDERS PASS
		{
			glClear(GL_DEPTH_BUFFER_BIT);
			
			DrawJeep();
			glReadPixels(0, 0, occluderMapWidth, occluderMapHeight,
						 GL_DEPTH_COMPONENT, GL_FLOAT, lightState.occluderDepthMapPixels);
			
			glDisable(GL_POLYGON_OFFSET_FILL);
			
			glBindTexture(GL_TEXTURE_2D, occluderTexID);
			glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
							 0, 0,
							 occluderMapWidth, occluderMapHeight, 0);
			
			lightState.occluderDepthTexID = occluderTexID;
		}
		
		// RESTORE VIEWPORT
		{
			//glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);	
			int x, y, w, h;
			GLUI_Master.get_viewport_area ( &x, &y, &w, &h );
			glViewport( x, y, w, h );
		}
		return lightState;
	}
	
private:
	void CameraFullPass()
	{
		glClear(GL_DEPTH_BUFFER_BIT);
		
		glShadeModel(GL_SMOOTH);
		glColorMask(1, 1, 1, 1);
		
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(camFovY, 1.0, 1.0, 600.0);
		
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(0.0f, 0.0f, -cameraDistance);
		glMultMatrixf(cameraMatrix);
		
		glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
		
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		
		glEnable(GL_LIGHTING);
		
		SetupTextureProjection(textureMatrix);
		
		if(1) glDisable(GL_TEXTURE_2D); // Shadow texture
		
		if(drawJeepPatches == 0)
			DrawJeep();
		
		if(1) glEnable(GL_TEXTURE_2D);
		DrawGround();
		
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);
		DrawLight();
	}
	void SetupTextureProjection(float *textureMatrix)
	{
		// Set up eye plane for projecting the shadow map on the scene
		{
			glEnable(GL_TEXTURE_GEN_S);
			glEnable(GL_TEXTURE_GEN_T);
			glEnable(GL_TEXTURE_GEN_R);
			glEnable(GL_TEXTURE_GEN_Q);
			glTexGenfv(GL_S, GL_EYE_PLANE, &textureMatrix[0]);
			glTexGenfv(GL_T, GL_EYE_PLANE, &textureMatrix[4]);
			glTexGenfv(GL_R, GL_EYE_PLANE, &textureMatrix[8]);
			glTexGenfv(GL_Q, GL_EYE_PLANE, &textureMatrix[12]);	
			glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
			glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
			glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
			glTexGeni(GL_Q, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
		}
		
		// Set modes for depth comparison and some other things
		if(0) // Shadow map
		{
			glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
			glEnable(GL_TEXTURE_2D);
			
			if(currentMode == MODE__DRAW_SHADOWED_SCENE)
			{
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
				glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			}
			
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, textureFilter);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, textureFilter);
			
			glBindTexture(GL_TEXTURE_2D, occluderTexID);
		}
		
		else // Soft shadow map
		{
			glBindTexture(GL_TEXTURE_2D, softShadowTextureID);
			
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, textureFilter);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, textureFilter);
			
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
			
			glColorMask(1, 1, 1, 1);
			
			glColor3f(1.0f, 1.0f, 1.0f);
		}
	}
	
	void VisualDebug()
	{
		if(currentMode == MODE__SHOW_PATCHES_VIEW)
		{
			glClear(GL_DEPTH_BUFFER_BIT);
			
			glColorMask(1, 1, 1, 1);
			
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			gluPerspective(camFovY, 1.0, 1.0, 100.0);
			
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			glTranslatef(0.0f, 0.0f, -cameraDistance);
			glMultMatrixf(cameraMatrix);
			glTranslatef(0.0f, lightState.zFar, 0.0f);
			
			glDisable(GL_TEXTURE_GEN_S);
			glDisable(GL_TEXTURE_GEN_T);
			glDisable(GL_TEXTURE_GEN_R);
			glDisable(GL_TEXTURE_GEN_Q);
		}
		
		else
		{
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();
			M3DMatrix44f tempMatrix;
			m3dInvertMatrix44(tempMatrix, lightModelview);
			glMultMatrixf(tempMatrix);
			glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
		}
		
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_LIGHTING);
		
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		
		glPointSize(3.0f);
		
		/*AAPatch *patches;
		 AAPatch *extents;
		 
		 patches = cpuPatches;
		 extents = cpuExtents;
		 
		 if(drawOccluderPatches && drawJeepPatches)
		 DrawPatches(patches, lightState.mapWidth*lightState.mapHeight, 1.0f, 0.9f, 0.7f);
		 if(drawPenumbraExtents)
		 DrawPatches(extents, lightState.mapWidth*lightState.mapHeight, 0.7f, 0.0f, 0.0f, false);
		 */
		
		//DrawPatch(lightState.lightArea, GL_QUADS);
		
		//if(drawNearAndFarPlanes)
		//	DrawNearAndFarPlanes(lightModelview);
		
		if(currentMode != MODE__SHOW_PATCHES_VIEW)
			glPopMatrix();
	}
	
	void DrawJeep(bool showAABB = false)
	{	
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glTranslatef(jeepPos[0], jeepPos[1], jeepPos[2]);
		//glRotatef(-45.0f, 0.0f, 1.0f, 0.0f);
		//glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
		glRotatef(180.0f, 0.0f, 1.0f, 0.0f);
		
		
		GLfloat jeepDiffuseColor[] = {1.0f, 0.7f, 0.25f, 0.1f};
		glMaterialfv(GL_FRONT, GL_DIFFUSE, jeepDiffuseColor);
		
		DrawMesh(jeep);
		
		if(showAABB)
		{
			glPushAttrib(GL_ENABLE_BIT);
			glDisable(GL_LIGHTING);
			glDisable(GL_TEXTURE_2D);
			glColor3f(1.0f, 1.0f, 1.0f);
			DrawAABB(*jeepAABB);
			glPopAttrib();
		}
		
		glPopMatrix();
	}
	
	void DrawGround()
	{
		const float scale = 2.0f;
		GLfloat groundColor[] = {0.075f * scale, 0.25f * scale, 0.0f * scale, 1.0f};
		glMaterialfv(GL_FRONT, GL_DIFFUSE, groundColor);
		
		glPushMatrix();
		glTranslatef(0.0f, -3.0f, 0.0f);
		glScalef(5.0f, 8.0f, 5.0f);	
		
		glEnable(GL_RESCALE_NORMAL);
		DrawMesh(ground);
		glDisable(GL_RESCALE_NORMAL);
		
		glPopMatrix();
	}
	
	void DrawLight()
	{
		glDisable(GL_LIGHTING);
		
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		
		M3DMatrix44f tempMatrix;
		m3dInvertMatrix44(tempMatrix, lightModelview);
		glMultMatrixf(tempMatrix);
		glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
		
		glColor3f(1.0f, 1.0f, 1.0f);
		//DrawPatch(lightState.lightArea, GL_QUADS);
		
		glPopMatrix();
	}
	
	void GenerateSoftShadowMap(const LightState &lightState, unsigned int softShadowTextureHandle){
	}	
	
	
public:
	float backColor[4];
	GLuint softShadowTexID;
	
	Mesh *jeep;
	AABB *jeepAABB;
	float jeepPos[4];
	
	Mesh *ground;
	Mesh *light;
	
	// TEXTURES ---------------------------------
	int textureFilter;
	GLuint occluderTexID;
	GLuint receiverTexID;
	GLuint softShadowTextureID;
	int occluderMapWidth;
	int occluderMapHeight;

	// LIGHT ------------------------------------
	GLfloat lightPos[4];
	M3DMatrix44f lightModelview, lightProjection, textureMatrix, tempMatrix;
	float lightFOVExageration;
	
	float farPlaneOffsetFactor;
	LightState lightState;
	int drawNearAndFarPlanes;
	int drawOccluderPatches;
	int drawPenumbraExtents;
	
	// CAMERA -----------------------------------
	GLfloat cameraDistance;
	GLfloat cameraMatrix[16];
	GLfloat camFovY;
};

#endif
#pragma once

static AAPatch zNearPatch, zFarPatch;


#ifdef GPU_MODE
inline float toDegree(const float &rad){ return rad*57.2957795f; };
inline float toRadian(const float &deg){ return deg*0.0174532925f;};
#endif
static Vertex3D ComputeApex(const AAPatch &lightSource, const AAPatch &microPatch)
{
	const float ratio = lightSource.width / microPatch.width;
	const float invRatio = ratio / (1.0f+ratio);
	
	return Vertex3D(microPatch.pos.x*invRatio, microPatch.z*invRatio, microPatch.pos.y*invRatio);
}

/*	
	Argument 'mode': which mode to use on glBegin(). If mode == NO_BEGIN, 
	glBegin and glEnd are not called (are expected to have already been called).
*/
#define NO_BEGIN 0xffffffff
static inline void DrawPatch(const AAPatch &patch, GLenum mode = GL_LINE_LOOP)
{
	const float
		halfWidth = patch.width*0.5f,
		halfDepth = patch.depth*0.5f,
		z = patch.z;

	const float
		xMin = patch.pos.x - halfWidth,
		xMax = patch.pos.x + halfWidth,
		zMin = patch.pos.y - halfDepth,
		zMax = patch.pos.y + halfDepth;

	if(mode != NO_BEGIN) glBegin(mode);
		glVertex3f(xMin, z, zMax);
		glVertex3f(xMax, z, zMax);
		glVertex3f(xMax, z, zMin);
		glVertex3f(xMin, z, zMin);
	if(mode != NO_BEGIN) glEnd();
}

static inline void DrawPatch4Sides(const AAPatch &patch)
{
	const float
		halfWidth = patch.width*0.5f,
		halfDepth = patch.depth*0.5f,
		z = patch.z;

	const float
		xMin = patch.pos.x - halfWidth,
		xMax = patch.pos.x + halfWidth,
		zMin = patch.pos.y - halfDepth,
		zMax = patch.pos.y + halfDepth;

	glVertex3f(xMin, z, zMax);
	glVertex3f(xMax, z, zMax);

	glVertex3f(xMax, z, zMax);
	glVertex3f(xMax, z, zMin);

	glVertex3f(xMax, z, zMin);
	glVertex3f(xMin, z, zMin);

	glVertex3f(xMin, z, zMin);
	glVertex3f(xMin, z, zMax);
}

static void DrawPatch(const AAPatch &patch, const Vertex3D &apex, GLenum mode = GL_LINE_LOOP, bool drawApexFaces = false)
{
	const float
		halfWidth = patch.width*0.5f,
		halfDepth = patch.depth*0.5f,
		z = patch.z;

	const float
		xMin = patch.pos.x - halfWidth,
		xMax = patch.pos.x + halfWidth,
		zMin = patch.pos.y - halfDepth,
		zMax = patch.pos.y + halfDepth;

	if(!drawApexFaces)
	{
		glBegin(mode);
			glTexCoord2f(0.0f, 0.0f);
			glVertex3f(xMin, z, zMin);
			
			glTexCoord2f(1.0f, 0.0f);
			glVertex3f(xMax, z, zMin);

			glTexCoord2f(1.0f, 1.0f);
			glVertex3f(xMax, z, zMax);

			glTexCoord2f(0.0f, 1.0f);
			glVertex3f(xMin, z, zMax);
		glEnd();

		glBegin(GL_LINES);
			glVertex3f(apex.x, apex.y, apex.z);
			glVertex3f(xMin, z, zMax);

			glVertex3f(apex.x, apex.y, apex.z);
			glVertex3f(xMax, z, zMax);

			glVertex3f(apex.x, apex.y, apex.z);
			glVertex3f(xMax, z, zMin);

			glVertex3f(apex.x, apex.y, apex.z);
			glVertex3f(xMin, z, zMin);
		glEnd();
	}

	else
	{
		glBegin(GL_TRIANGLES);
			glVertex3f(apex.x, apex.y, apex.z);
			glVertex3f(xMin, z, zMax);
			glVertex3f(xMax, z, zMax);

			glVertex3f(apex.x, apex.y, apex.z);
			glVertex3f(xMax, z, zMax);
			glVertex3f(xMax, z, zMin);

			glVertex3f(apex.x, apex.y, apex.z);
			glVertex3f(xMax, z, zMin);
			glVertex3f(xMin, z, zMin);

			glVertex3f(apex.x, apex.y, apex.z);
			glVertex3f(xMin, z, zMin);
			glVertex3f(xMin, z, zMax);
		glEnd();
	}
}

static void DrawNearAndFarPlanes(M3DMatrix44f lightModelview, float fovY)
{
	// CREATE NEAR AND FAR PLANES
	{
		const float halfFovY = toRadian(fovY)*0.5f;
		const float 
			fovSin = sinf(halfFovY),
			fovCos = cosf(halfFovY);

		const float 
			farWidth  = ((lightState.zFar *fovSin)/fovCos)*2.0f,
			nearWidth = ((lightState.zNear*fovSin)/fovCos)*2.0f;

		zNearPatch.width = zNearPatch.depth = nearWidth;
		zNearPatch.pos = Vertex2D(0.0f, 0.0f);
		zNearPatch.z = lightState.zNear;
		zFarPatch.width = zFarPatch.depth = farWidth;
		zFarPatch.pos = Vertex2D(0.0f, 0.0f);
		zFarPatch.z = lightState.zFar;
	}

	glDisable(GL_LIGHTING);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
/*
	M3DMatrix44f tempMatrix;
	m3dInvertMatrix44(tempMatrix, lightModelview);
	glMultMatrixf(tempMatrix);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
*/
	glScalef(1.0f, -1.0f, -1.0f);

	glColor3f(1.0f, 1.0f, 1.0f);

	const Vertex3D apex(0.0f, 0.0f, 0.0f);
	DrawPatch(zNearPatch, apex);
	
	/*glBegin(GL_LINES);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(1.0f, 0.0f, 0.0f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glVertex3f(0.0f, 0.0f, 1.0f);
	glEnd();*/

	glEnable(GL_TEXTURE_2D);
	DrawPatch(zFarPatch, apex, GL_QUADS);
	glDisable(GL_TEXTURE_2D);

	if(0)
	{
		glBegin(GL_POINTS);
		const float step = zFarPatch.width/lightState.mapWidth;
		for(float x=-zFarPatch.width/2.0f; x<zFarPatch.width/2.0f; x+=step)
			for(float y=-zFarPatch.depth/2.0f; y<zFarPatch.depth/2.0f; y+=step)
				glVertex3f(x, zFarPatch.z, y);
		glEnd();
	}
	glPopMatrix();

}
static void DrawPatches(AAPatch *patches, int numPatches, float r, float g, float b, bool showPyramid = false)
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glScalef(1.0f, -1.0f, -1.0f);

	// DRAW MICRO PATCHES LINE BORDERS
	glColor4f(r, g, b, 1.f);

	if(!showPyramid)
	{
		glBegin(GL_LINES);
		for(int i=0; i<numPatches; ++i)
			if(patches[i].valid)
				DrawPatch4Sides(patches[i]);
		glEnd();
	}
	else for(int i=0; i<numPatches; ++i) 
		if(patches[i].valid)
		{
			const Vertex3D apex = ComputeApex(lightState.lightArea, patches[i]);
			DrawPatch(patches[i], apex);
			DrawPatch(lightState.lightArea, apex);
		}
	
	glPopMatrix();
}

static void DrawTexture(const LightState &lightState, void *pixels)
{
	static GLuint testTexture = 0;
	if(testTexture == 0)
		glGenTextures(1, &testTexture);

	glBindTexture(GL_TEXTURE_2D, testTexture);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		lightState.mapWidth, lightState.mapHeight, 0, 
		GL_DEPTH_COMPONENT, GL_FLOAT, pixels);

	glEnable(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glDisable(GL_TEXTURE_GEN_R);
	glDisable(GL_TEXTURE_GEN_Q);

	glColorMask(1, 1, 1, 1);
	glDisable(GL_DEPTH_TEST);

	glColor3f(1.0f, 1.0f, 1.0f);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 1.0f);
		glVertex2f(-1.0f, 1.0f);
		
		glTexCoord2f(1.0f, 1.0f);
		glVertex2f(-0.5f, 1.0f);
		
		glTexCoord2f(1.0f, 0.0f);
		glVertex2f(-0.5f, 0.5f);
		
		glTexCoord2f(0.0f, 0.0f);
		glVertex2f(-1.0f, 0.5f);
	glEnd();
}


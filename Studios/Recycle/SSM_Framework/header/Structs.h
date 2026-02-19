#pragma once

struct Vertex2D
{
	Vertex2D() {}
	Vertex2D(const float x, const float y) : x(x), y(y) {}
	inline void operator- (const Vertex2D &v) { x -= v.x; y -= v.y; }
	inline Vertex2D operator* (const float v) const { return Vertex2D(x*v, y*v); }
	float x, y;
};

struct Vertex3D
{
	Vertex3D() {}
	Vertex3D(const float x, const float y, const float z) : x(x), y(y), z(z) {}
	inline void operator- (const Vertex3D &v) { x -= v.x; y -= v.y; z -= v.z; }
	float x, y, z;
};

struct AAPatch
{
	AAPatch() {}
	AAPatch(float width, float depth, float z, const Vertex2D &pos)
		: width(width), depth(depth), z(z), pos(pos) {}
	
	float width;
	float depth;		
	float z;
	Vertex2D pos;
	bool valid;
};

typedef struct LightState
{
	unsigned int occluderDepthTexID;
	unsigned int receiverDepthTexID;

	unsigned int mapWidth;
	unsigned int mapHeight;

	float *occluderDepthMapPixels;
	float *receiverDepthMapPixels;
	
	float zNear;
	float zFar;
	float fovY;

	AAPatch lightArea;
} LightState;

typedef unsigned int TexHandle;

typedef struct SoftShadowMapGeneratorInput
{
	TexHandle occluder;	// depth map
	TexHandle receiver; // depth map
	TexHandle SSM;		
	int texWidth;
	int texHeight;

	float zNear;
	float zFar;
	float fovY;

} SoftShadowMapGeneratorInput;

typedef SoftShadowMapGeneratorInput SSMInput;
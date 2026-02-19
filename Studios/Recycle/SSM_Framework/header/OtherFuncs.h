#ifndef FUNCS_H
#define FUNCS_H

#include "MeshStructs.h"
//#include "Matrix.h"

inline const float Length(const float &x, const float &y, const float &z)
{ return(sqrtf(x*x + y*y + z*z)); }

inline const float Distance(const Vertex &A, const Vertex &B)
{ return Length(A.X-B.X, A.Y-B.Y, A.Z-B.Z); }

inline const float RetrieveRadius(AABB &aabb)
{ return Length(aabb.halfWidth, aabb.halfHeight, aabb.halfDepth); }

inline void MultiplyVector(Vertex &v, const float *m)
{
	const float x = v.X, y = v.Y, z = v.Z;
	/*
	 v.X = x*m[0] + y*m[1] + z*m[2] + m[3];
	 v.Y = x*m[4] + y*m[5] + z*m[6] + m[7];
	 v.Z = x*m[8] + y*m[9] + z*m[10] + m[11];
	 */
	v.X = x*m[0] + y*m[4] + z*m[8] + m[12];
	v.Y = x*m[1] + y*m[5] + z*m[9] + m[13];
	v.Z = x*m[2] + y*m[6] + z*m[10] + m[14];
}

inline void ComputePatch(const AreaLight &light, const float distance,
	const float scale, PointArea &result)
{
	const float
		halfScale = scale*0.5f;
	const float 
		halfWidth = light.width*halfScale,
		halfHeight = light.height*halfScale;

	result[0] = Vertex(-halfWidth, halfHeight, -distance);
	result[1] = Vertex(halfWidth, halfHeight, -distance);
	result[2] = Vertex(halfWidth, -halfHeight, -distance);
	result[3] = Vertex(-halfWidth, -halfHeight, -distance);
	
	for(int i=0; i<4; ++i)
		MultiplyVector(result[i], &light.modelView[0]);
}

#endif
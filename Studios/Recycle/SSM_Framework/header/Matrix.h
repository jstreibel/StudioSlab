#pragma once

#include "OBJ_Reader.h"

inline float* M4x4_LoadIdentity(float *m)
{
	for(int i=0; i<4; ++i)
		for(int j=0; j<4; ++j)
			m[i+(j*4)] = (i==j) ? 1.0f : 0.0f;

	return m;
}

inline float* M4x4_Translate(float *m, float x, float y, float z)
{
	m[12] += x;
	m[13] += y;
	m[14] += z;

	return m;
}

inline float* M4x4_Scale(float *m, float x, float y, float z)
{ m[0] *= x; m[5] *= y; m[10] *= z; return m; }

/*inline float* M4x4_Multiply(float *m1, float *m2, float *dest)
{
	float sum;
	for(int i=0; i<4; ++i)
	{
		for(int j=0; j<4; ++j)
		{
			sum = 0.0f;
			for(int k=0; k<4; ++k) sum+=m1[i*4+k]*m2[k*4+j];
			dest[i*4 + j] = sum;
		}
	}

	return dest;
}*/
/*
inline float* M4x4_Transpose(float *m, float *dest)
{
	for(int i=0; i<4; ++i)
		for(int j=0; j<4; ++j)
			dest[j*4+i] = m[i*4+j];

	return dest;
}*/

Vertex &M4x4_MultiplyVector(Vertex &v, const float *m)
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
	return v;
}
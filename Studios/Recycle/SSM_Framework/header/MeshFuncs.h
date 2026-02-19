#ifndef DRAW_MESH_H
#define DRAW_MESH_H

#ifdef WIN32
#include <Windows.h>
#endif
#include <math.h>
#include "OBJ_Reader.h"
#include "MeshStructs.h"

#include <GLUT/GLUT.h>

#define PI_DIV_180_f 0.017453292519943296f
#define INV_PI_DIV_180_f 57.2957795130823229f
#define DegToRad_Float(x)	((x)*PI_DIV_180_f)
#define RadToDeg_Float(x)	((x)*INV_PI_DIV_180_f)

static void DrawMesh(Mesh *mesh, GLenum mode = GL_TRIANGLES)
{
	const Vertex *vertices		= &mesh->vertices[0];
	const Vertex *normals			= mesh->normals.size() ? &mesh->normals[0] : NULL;
	const TextureCoord *texCoords	= mesh->tex_coords.size() ? &mesh->tex_coords[0] : NULL;
	
	const Group *groups			= &mesh->groups[0];
	const size_t numGroups		= mesh->groups.size();

	glBegin(mode);
	{
		for(size_t g=0; g<numGroups; ++g)
		{
			if(groups[g].faces.size()==0) continue;

			const Face *faces = &groups[g].faces[0];
			const size_t numFaces = groups[g].faces.size();

			for(size_t f=0; f<numFaces; ++f)
			{
				// TO-DO: hmm... Three-sided faces only.. Tidak bagus.
				// const size_t numSides = faces[f].vertices.size();
				const size_t numSides = 3;
				for(size_t s=0; s<numSides; ++s)
				{
					const Vertex *v			= &vertices[faces[f].vertices[s]-1];
					const Vertex *n			= normals ? &normals[faces[f].normals[s]-1] : NULL;
					const TextureCoord *c	= texCoords ? &texCoords[faces[f].tex_coord[s]-1] : NULL;

					if(normals)		glNormal3f		(n->X, n->Y, n->Z);
					if(texCoords)	glTexCoord3f	(c->U, c->V, c->W);
					glVertex3f		(v->X, v->Y, v->Z);
				}
			}
		}
	}
	glEnd();
}

static void DrawAABB(const AABB &aabb)
{
	const float 
		xMin = -aabb.halfWidth + aabb.pos.X,
		xMax =  aabb.halfWidth + aabb.pos.X,
		yMin = -aabb.halfHeight + aabb.pos.Y,
		yMax = aabb.halfHeight + aabb.pos.Y,
		zMin = -aabb.halfDepth  + aabb.pos.Z,
		zMax = aabb.halfDepth  + aabb.pos.Z;
	
	glBegin(GL_LINES);
	{
		glVertex3f(xMin, yMin, zMax);
		glVertex3f(xMin, yMin, zMin);

		glVertex3f(xMin, yMin, zMin);
		glVertex3f(xMax, yMin, zMin);

		glVertex3f(xMax, yMin, zMin);
		glVertex3f(xMax, yMin, zMax);

		glVertex3f(xMax, yMin, zMax);
		glVertex3f(xMin, yMin, zMax);

		glVertex3f(xMin, yMin, zMax);
		glVertex3f(xMin, yMax, zMax);

		glVertex3f(xMin, yMax, zMax);
		glVertex3f(xMin, yMax, zMin);

		glVertex3f(xMin, yMax, zMin);
		glVertex3f(xMax, yMax, zMin);

		glVertex3f(xMax, yMax, zMin);
		glVertex3f(xMax, yMax, zMax);
		
		glVertex3f(xMax, yMax, zMax);
		glVertex3f(xMin, yMax, zMax);

		glVertex3f(xMin, yMax, zMin);
		glVertex3f(xMin, yMin, zMin);

		glVertex3f(xMax, yMax, zMin);
		glVertex3f(xMax, yMin, zMin);

		glVertex3f(xMax, yMax, zMax);
		glVertex3f(xMax, yMin, zMax);
	}
	glEnd();
}

static void Draw1x1XYAlignedSquarePlane(GLuint mode = GL_LINE_LOOP)
{
	glBegin(mode);
		glVertex2f(-0.5f,  0.5f);
		glVertex2f( 0.5f,  0.5f);
		glVertex2f( 0.5f, -0.5f);
		glVertex2f(-0.5f, -0.5f);
	glEnd();
}

static AABB FindMeshAABB(Mesh *mesh)
{
	float xMax = mesh->vertices[0].X,
		  xMin = mesh->vertices[0].X,
		  yMax = mesh->vertices[0].Y,
		  yMin = mesh->vertices[0].Y,
		  zMax = mesh->vertices[0].Z,
		  zMin = mesh->vertices[0].Z;

	for(size_t i=0; i<mesh->vertices.size(); ++i)
	{
		Vertex &v = mesh->vertices[i];
		
		if(v.X > xMax) xMax = v.X;
		else if(v.X < xMin) xMin = v.X;

		if(v.Y > yMax) yMax = v.Y;
		else if(v.Y < yMin) yMin = v.Y;

		if(v.Z > zMax) zMax = v.Z;
		else if(v.Z < zMin) zMin = v.Z;
	}

	const Vertex pos((xMin+xMax)*0.5f, (yMin+yMax)*0.5f, (zMin+zMax)*0.5f);

	return AABB(pos.X - xMin, pos.Y - yMin, pos.Z - zMin, pos);
};


#endif
#pragma once

#include <Vector>
#include <math.h>

#include "math3d.h"

struct TextureCoord{
	float U, V, W;
};

struct Vertex{
	Vertex(void) {}
	Vertex(const float &X, const float &Y, const float &Z)
		: X(X), Y(Y), Z(Z) {}
	Vertex(const float V[3])
		: X(V[0]), Y(V[1]), Z(V[2]) {}
	float X, Y, Z;
};

typedef Vertex Vector3D;

class Face{
public:
	Face() : vertices(0), normals(0), tex_coord(0)
	{
		vertices.reserve(3);
		normals.reserve(3);
		tex_coord.reserve(3);
	}

	inline void addVertexIndex( unsigned int vertex_index ){
		vertices.push_back( vertex_index );
	}
	inline unsigned int getVertexIndex( unsigned int position ){
		if( position >= vertices.size( ) )
		{
			return 0;
		}
		return vertices.at( position );
	}

	inline void addNormalIndex( unsigned int normal_index ){
		normals.push_back( normal_index );
	}
	inline unsigned int getNormalIndex( unsigned int position ){
		if( normals.size( ) >= position )
		{
			return 0;
		}
		return normals.at( position );
	}
	inline void addTextureCoordinateIndex( unsigned int tex_coord_index ){
		tex_coord.push_back( tex_coord_index );
	}
	inline unsigned int getTextureCoordinateIndex( unsigned int position ){
		if( tex_coord.size() >= position )
		{
			return 0;
		}
		return tex_coord.at( position );
	}
	// getNumberOfSides( ) >>> retorna o numero de vertices que atualmente compõem a face.
	inline size_t getNumberOfSides( void ){
		return vertices.size( );
	}
//private:
	std::vector<unsigned int> vertices;
	std::vector<unsigned int> normals;
	std::vector<unsigned int> tex_coord;
};

class Group{
public:
	Group()	: faces(0)
	{
		faces.reserve(15000);
	}
	inline void addFace( Face new_face ){
		faces.push_back( new_face );
	}
	inline size_t getNumberOfFaces( void ){
		return faces.size( );
	}
	inline Face getFace( unsigned int face_index ){
		return faces.at( face_index );
	}
//private:
	std::vector<Face> faces;
};

class Mesh{
public:

	Mesh() : vertices(0), normals(0), tex_coords(0), groups(0)
	{
		vertices.reserve(15000);
		normals.reserve(15000);
		tex_coords.reserve(15000);

		groups.reserve(10);
	}

	inline void addVertex( Vertex new_vertex ){	vertices.push_back( new_vertex );	}
	inline Vertex getVertex( unsigned int index ){	return vertices.at( index );		}
	inline size_t getNumberOfVertices( void ){	return vertices.size( );	}

	inline void addNormal( Vertex new_normal ){	normals.push_back( new_normal );	}
	inline Vertex getNormal( unsigned int index ){	return normals.at( index );			}
	inline size_t getNumberOfNormals( void ){	return normals.size( );			}

	inline void addTextureCoordinate( TextureCoord new_tex_coord ){	tex_coords.push_back( new_tex_coord );	}
	inline TextureCoord getTextureCoordinate( unsigned int index ){		return tex_coords.at( index );			}
	inline size_t getNumberOfTexCoords( void ){	return tex_coords.size( );	}

	inline void addGroup( Group new_group ){	groups.push_back( new_group );	}
	inline Group getGroup( unsigned int index ){		return groups.at( index );		}
	inline size_t getNumberOfGroups( void ){	return groups.size( );		}
//private:
	std::vector<Vertex> vertices;
	std::vector<Vertex> normals;
	std::vector<TextureCoord> tex_coords;

	std::vector<Group> groups;
};

struct AABB
{
	AABB() {}
	AABB(const float &halfWidth, const float &halfHeight, const float &halfDepth, const Vertex &pos)
		: halfWidth(halfWidth), halfHeight(halfHeight), halfDepth(halfDepth), pos(pos) {}

	float halfWidth, halfHeight, halfDepth;
	Vertex pos;
};

struct SquarePlane
{
	Vertex v[4];
};

typedef Vertex PointArea[4];

struct AreaLight
{
	float width, height;
	float modelView[16];
};
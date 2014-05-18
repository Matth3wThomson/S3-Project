/******************************************************************************
Class:Mesh
Implements:
Author:Rich Davison	<richard.davison4@newcastle.ac.uk>
Description:Wrapper around OpenGL primitives, geometry and related 
OGL functions.

There's a couple of extra functions in here that you didn't get in the tutorial
series, to draw debug normals and tangents. 


-_-_-_-_-_-_-_,------,   
_-_-_-_-_-_-_-|   /\_/\   NYANYANYAN
-_-_-_-_-_-_-~|__( ^ .^) /
_-_-_-_-_-_-_-""  ""   

*//////////////////////////////////////////////////////////////////////////////

#pragma once

#include <vector>
#include <string>
#include <fstream>
#include "../nclgl/Vector2.h"
#include "../nclgl/OGLRenderer.h"

//#include "FreqBar.h"


using std::ifstream;
using std::string;

//A handy enumerator, to determine which member of the bufferObject array
//holds which data
enum MeshBuffer {
	VERTEX_BUFFER	=0,
	COLOUR_BUFFER	=1, 
	TEXTURE_BUFFER	,
	INDEX_BUFFER	,
	MAX_BUFFER
};

class Mesh	{
public:
	friend class FreqBar;

	Mesh(void);
	virtual ~Mesh(void);

	virtual void Draw();

	//Generates a single triangle, with RGB colours
	static Mesh*	GenerateTriangle();

	static Mesh* GenerateCircle(int points, float radius);

	static Mesh* GenerateBars(int points);

	static Mesh* GenerateBar(int bar, int noOfBars, float radius);

	static Mesh*	LoadMeshFile(const string &filename);
	
	//Generates a single white quad, going from -1 to 1 on the x and z axis.
	static Mesh*	GenerateQuad(const Vector4& colour);

	static Mesh*	GenerateQuadPatch();

	static Mesh*	GenerateQuadAlt();

	//Sets the Mesh's diffuse map. Takes an OpenGL texture 'name'
	void	SetTexture(GLuint tex)	{texture = tex;}
	//Gets the Mesh's diffuse map. Returns an OpenGL texture 'name'
	GLuint  GetTexture()			{return texture;}

	GLuint	type;

protected:
	//Buffers all VBO data into graphics memory. Required before drawing!
	void	BufferData();


	//Generates normals for all facets. Assumes geometry type is GL_TRIANGLES...
	void	GenerateNormals();

	//Generates tangents for all facets. Assumes geometry type is GL_TRIANGLES...
	void	GenerateTangents();
	//Helper function for GenerateTangents
	Vector3 GenerateTangent(const Vector3 &a,const Vector3 &b,const Vector3 &c,const Vector2 &ta,const Vector2 &tb,const Vector2 &tc);

	//VAO for this mesh
	GLuint	arrayObject;
	//VBOs for this mesh
	GLuint	bufferObject[MAX_BUFFER];
	//Number of vertices for this mesh
	GLuint	numVertices;
	//Primitive type for this mesh (GL_TRIANGLES...etc)
	
	//OpenGL texture name for the diffuse map
	GLuint	texture;

	//Stuff introduced later on in the tutorials!!

	//Number of indices for this mesh
	GLuint			numIndices;

	//Pointer to vertex position attribute data (badly named...?)
	Vector3*		vertices;
	//Pointer to vertex colour attribute data
	Vector4*		colours;
	//Pointer to vertex texture coordinate attribute data
	Vector2*		textureCoords;
	//Pointer to vertex indices attribute data
	unsigned int*	indices;
};


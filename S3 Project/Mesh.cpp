#include "Mesh.h"

Mesh::Mesh(void)	{
	glGenVertexArrays(1, &arrayObject);

	for(int i = 0; i < MAX_BUFFER; ++i) {
		bufferObject[i] = 0;
	}

	texture		 = 0;
	numVertices  = 0;
	type		 = GL_TRIANGLES;

	//Later tutorial stuff
	numIndices    = 0;
	vertices	  = NULL;
	textureCoords = NULL;
	indices		  = NULL;
	colours		  = NULL;
}

Mesh::~Mesh(void)	{
	glDeleteVertexArrays(1, &arrayObject);			//Delete our VAO
	glDeleteBuffers(MAX_BUFFER, bufferObject);		//Delete our VBOs

	glDeleteTextures(1,&texture);					//We'll be nice and delete our texture when we're done with it

	//Later tutorial stuff
	delete[]vertices;
	delete[]indices;
	delete[]textureCoords;
	delete[]colours;
}

void Mesh::Draw(){
	glBindVertexArray(arrayObject);
	if(bufferObject[INDEX_BUFFER]) {
		glDrawElements(type, numIndices, GL_UNSIGNED_INT, 0);
	}
	else{
		glDrawArrays(type, 0, numVertices);	//Draw the triangle!
	}
	glBindVertexArray(0);	
}

Mesh* Mesh::GenerateTriangle()	{
	Mesh*m = new Mesh();
	m->numVertices = 3;

	m->vertices = new Vector3[m->numVertices];
	m->vertices[0] = Vector3(0.0f,	0.5f,	0.0f);
	m->vertices[1] = Vector3(0.5f,  -0.5f,	0.0f);
	m->vertices[2] = Vector3(-0.5f, -0.5f,	0.0f);

	m->textureCoords = new Vector2[m->numVertices];
	m->textureCoords[0] = Vector2(0.5f,	0.0f);
	m->textureCoords[1] = Vector2(1.0f,	1.0f);
	m->textureCoords[2] = Vector2(0.0f,	1.0f);

	m->colours = new Vector4[m->numVertices];
	m->colours[0] = Vector4(1.0f, 0.0f, 0.0f,1.0f);
	m->colours[1] = Vector4(0.0f, 1.0f, 0.0f,1.0f);
	m->colours[2] = Vector4(0.0f, 0.0f, 1.0f,1.0f);

	m->BufferData();

	return m;
}

Mesh* Mesh::GenerateCircle(int points, float radius){
	Mesh* m = new Mesh();
	m->numVertices = points+2;

	m->vertices = new Vector3[m->numVertices];
	m->colours = new Vector4[m->numVertices];

	m->vertices[0] = Vector3(0.0f, 0.0f, 0.0f);
	m->colours[0] = Vector4(0.5f, 0.5f, 0.5f, 1.0f);

	for (unsigned int i=1; i<m->numVertices; ++i){
		m->vertices[i] = Vector3(radius * cos(DegToRad((float) (i-1) * 360/points)), radius * sin(DegToRad((float) (i-1) * 360/points)), 0.0f);

		float factor = (float) (i-1) / points;

		m->colours[i] = Vector4(factor, 1 - abs(2 * factor - 1) ,1 - factor, 1.0f);
	}

	/*//DEBUGGING
	std::cout << m->numVertices;

	for (int i=0; i<m->numVertices; ++i){
	std::cout << "Vertex: " << m->vertices[i] << std::endl;
	std::cout << "Colour: " << m->colours[i] << std::endl;
	}*/

	//Index buffering (like a triangle fan)
	m->numIndices = (3 * (m->numVertices - 1) + 3);
	int * x = new int[7];
	m->indices = new unsigned int[m->numIndices];

	int iBuff = 0;

	for (unsigned int i=1; i<m->numVertices-1; ++i){
		m->indices[iBuff] = 0;
		m->indices[iBuff+1] = i;
		m->indices[iBuff+2] = i+1;

		iBuff += 3;
	}

	m->BufferData();

	return m;
}

Mesh* Mesh::GenerateBars(int points){
	Mesh* m = new Mesh();

	//Each bar should be a quad.
	m->numVertices = points*4;

	//Each bar should have a colour
	m->vertices = new Vector3[m->numVertices];
	m->colours = new Vector4[m->numVertices];

	Vector3 center(0.0f, 0.0f, 0.0f);

	float radius = 0.5f;

	int point = 0;

	//Loop on a per bar step
	for (unsigned int i=0; i<m->numVertices; i += 4){
		//Create the bottom left part of the bar
		m->vertices[i] = Vector3(radius * cos(DegToRad((float) i * 360/m->numVertices )), radius * sin(DegToRad((float) (i) * 360/m->numVertices)), 0.0f);

		float factor = (float) i / m->numVertices;

		m->colours[i] = Vector4(factor, 1 - abs(2 * factor - 1) ,1 - factor, 1.0f);

		//Create the bottom right part of the bar
		m->vertices[i+1] = Vector3(radius * cos(DegToRad((float) (i+1) * 360/ m->numVertices )), radius * sin(DegToRad((float) (i+1) * 360/m->numVertices)), 0.0f);

		factor = (float) (i+1) / m->numVertices;

		m->colours[i+1] = Vector4(factor, 1 - abs(2 * factor - 1) ,1 - factor, 1.0f);

		//Create the top left part of the bar TODO: sort this out
		m->vertices[i+2] = m->vertices[i];
		m->colours[i+2] = m->colours[i];

		//Create the top right part of the bar
		m->vertices[i+3] = m->vertices[i+1];
		m->colours[i+3] = m->colours[i+1];

		point++;

	}

	/*//DEBUGGING
	std::cout << "BARS: " <<  m->numVertices;

	for (int i=0; i<m->numVertices; ++i){
	std::cout << "Vertex: " << m->vertices[i] << std::endl;
	std::cout << "Colour: " << m->colours[i] << std::endl;
	}*/

	//Index buffering (like triangle strip every 4 points)
	m->numIndices = points * 6; //(2 triangles per point, 3 indices per triangle)
	m->indices = new unsigned int[m->numIndices];

	int vertCount =0;

	for(unsigned int i = 0; i< m->numIndices; i+=6){
		m->indices[i] = vertCount;
		m->indices[i+1] = vertCount+1;
		m->indices[i+2] = vertCount+2;
		m->indices[i+3] = vertCount+1;
		m->indices[i+4] = vertCount+2;
		m->indices[i+5] = vertCount+3;

		vertCount += 4;
	}

	m->BufferData();

	return m;
}

Mesh* Mesh::GenerateBar(int bar, int noOfBars, float radius){
	//Create the mesh
	Mesh* m = new Mesh();

	//It should have 4 vertices
	m->numVertices = 4;
	m->vertices = new Vector3[m->numVertices];
	m->colours = new Vector4[m->numVertices];

	m->numIndices = 6;
	m->indices = new unsigned int[m->numIndices];

	float factor = (float) bar / noOfBars;

	//Create the vertices for the bar
	m->vertices[0] = Vector3(radius * cos(DegToRad( factor * 360 ) ), radius * sin(DegToRad( factor * 360 )), -0.05f);
	m->vertices[1] = Vector3(radius * cos(DegToRad( (factor * 360) + 360 / noOfBars )), radius * sin(DegToRad( (factor * 360)  + 360 / noOfBars)), -0.05f);
	m->vertices[2] = m->vertices[0]; // + (m->vertices[0] * radius);
	m->vertices[3] = m->vertices[1]; // + (m->vertices[1] * radius);

	/*//Attempt 1!
	Vector3 temp = m->vertices[0];
	m->vertices[0] -= m->vertices[1];
	m->vertices[1] -= temp;*/

	/*//Attempt 2!
	Vector3 midPoint((m->vertices[0].x + m->vertices[1].x) / 2, (m->vertices[0].y + m->vertices[1].y) /2, 0.0f); 
	m->vertices[0] -= midPoint;
	m->vertices[1] -= midPoint;*/

	//Attempt 3!
	m->vertices[1] -= m->vertices[0];
	m->vertices[0] -= m->vertices[0];

	//Create the colours for the bar
	for (unsigned int i=0; i<m->numVertices; ++i){
		m->colours[i] = Vector4(factor, 1 - abs(2 * factor - 1) ,1 - factor, 1.0f);
	}

	//Create the indices for the bar
	m->indices[0] = 0;
	m->indices[1] = 1;
	m->indices[2] = 2;
	m->indices[3] = 1;
	m->indices[4] = 2;
	m->indices[5] = 3;

	//Send the buffer data to the GPU
	m->BufferData();

	//Return the new pointer
	return m;
};

Mesh* Mesh::GenerateQuadPatch()	{
	Mesh* m			= new Mesh();

	m->numVertices	= 4;
	m->type			= GL_PATCHES;

	m->vertices			= new Vector3[m->numVertices];
	m->textureCoords	= new Vector2[m->numVertices];


	m->vertices[0] = 	Vector3(-1.0f, -1.0f, 0.0f);
	m->vertices[1] = 	Vector3(-1.0f,	1.0f, 0.0f);
	m->vertices[2] = 	Vector3(1.0f, -1.0f, 0.0f);
	m->vertices[3] = 	Vector3(1.0f,  1.0f, 0.0f);

	m->textureCoords[0] = Vector2(0.0f,	1.0f);
	m->textureCoords[1] = Vector2(0.0f,	0.0f);
	m->textureCoords[2] = Vector2(1.0f,	1.0f);
	m->textureCoords[3] = Vector2(1.0f,	0.0f);

	m->BufferData();

	return m;
}

Mesh*	Mesh::LoadMeshFile(const string &filename) {
	ifstream f(filename);

	if(!f) {
		return NULL;
	}

	Mesh*m = new Mesh();
	f >> m->numVertices;

	int hasTex = 0;
	int hasColour = 0;

	f >> hasTex;
	f >> hasColour;

	m->vertices = new Vector3[m->numVertices];

	if(hasTex) {
		m->textureCoords = new Vector2[m->numVertices];
		m->colours		 = new Vector4[m->numVertices];
	}

	for(int i = 0; i << m->numVertices; ++i) {
		f >> m->vertices[i].x;
		f >> m->vertices[i].y;
		f >> m->vertices[i].z;

		if(hasTex) {
			f >> m->textureCoords[i].x;
			f >> m->textureCoords[i].y;
		}

		if(hasColour) {
			char r,g,b,a;

			f >> r;
			f >> g;
			f >> b;
			f >> a;

			m->colours[i] = Vector4(r / 255.0f, g / 255.0f,b / 255.0f,a / 255.0f);
		}
	}
	return m;
}

Mesh* Mesh::GenerateQuad(const Vector4& colour)	{
	Mesh* m = new Mesh();

	m->numVertices = 4;
	m->type = GL_TRIANGLE_STRIP;

	m->vertices			= new Vector3[m->numVertices];
	m->textureCoords	= new Vector2[m->numVertices];
	m->colours			= new Vector4[m->numVertices];

	m->vertices[0] = 	Vector3(-1.0f, -1.0f, 0.0f);
	m->vertices[1] = 	Vector3(-1.0f,	1.0f, 0.0f);
	m->vertices[2] = 	Vector3(1.0f, -1.0f, 0.0f);
	m->vertices[3] = 	Vector3(1.0f,  1.0f, 0.0f);

	m->textureCoords[0] = Vector2(0.0f,	1.0f);
	m->textureCoords[1] = Vector2(0.0f,	0.0f);
	m->textureCoords[2] = Vector2(1.0f,	1.0f);
	m->textureCoords[3] = Vector2(1.0f,	0.0f);

	for(int i = 0; i < 4; ++i) {
		m->colours[i] = colour;
	}

	m->BufferData();

	return m;
}


Mesh* Mesh::GenerateQuadAlt()	{
	Mesh* m = new Mesh();

	m->numVertices = 4;
	m->type = GL_TRIANGLE_STRIP;

	m->vertices			= new Vector3[m->numVertices];
	m->textureCoords	= new Vector2[m->numVertices];
	m->colours			= new Vector4[m->numVertices];

	m->vertices[0] = 	Vector3(0.0f, 0.0f, 0.0f);
	m->vertices[1] = 	Vector3(0.0f, 1.0f, 0.0f);
	m->vertices[2] = 	Vector3(1.0f, 0.0f, 0.0f);
	m->vertices[3] = 	Vector3(1.0f,  1.0f, 0.0f);

	m->textureCoords[0] = Vector2(0.0f,	0.0f);
	m->textureCoords[1] = Vector2(0.0f,	1.0f);
	m->textureCoords[2] = Vector2(1.0f,	0.0f);
	m->textureCoords[3] = Vector2(1.0f,	1.0f);

	for(int i = 0; i < 4; ++i) {
		m->colours[i] = Vector4(1.0f, 1.0f,1.0f,1.0f);
	}

	m->BufferData();

	return m;
}

void	Mesh::BufferData()	{
	glBindVertexArray(arrayObject);

	//Buffer vertex data
	glGenBuffers(1, &bufferObject[VERTEX_BUFFER]);
	glBindBuffer(GL_ARRAY_BUFFER, bufferObject[VERTEX_BUFFER]);
	glBufferData(GL_ARRAY_BUFFER, numVertices*sizeof(Vector3), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(VERTEX_BUFFER, 3, GL_FLOAT, GL_FALSE, 0, 0); 
	glEnableVertexAttribArray(VERTEX_BUFFER);

	////Buffer texture data
	if(textureCoords) {
		glGenBuffers(1, &bufferObject[TEXTURE_BUFFER]);
		glBindBuffer(GL_ARRAY_BUFFER, bufferObject[TEXTURE_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, numVertices*sizeof(Vector2), textureCoords, GL_STATIC_DRAW);
		glVertexAttribPointer(TEXTURE_BUFFER, 2, GL_FLOAT, GL_FALSE, 0, 0); 
		glEnableVertexAttribArray(TEXTURE_BUFFER);
	}

	//buffer colour data
	if (colours)	{
		glGenBuffers(1, &bufferObject[COLOUR_BUFFER]);
		glBindBuffer(GL_ARRAY_BUFFER, bufferObject[COLOUR_BUFFER]);
		glBufferData(GL_ARRAY_BUFFER, numVertices*sizeof(Vector4), colours, GL_STATIC_DRAW);
		glVertexAttribPointer(COLOUR_BUFFER, 4, GL_FLOAT, GL_FALSE, 0, 0); 
		glEnableVertexAttribArray(COLOUR_BUFFER);
	}

	//buffer index data
	if(indices) {
		glGenBuffers(1, &bufferObject[INDEX_BUFFER]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferObject[INDEX_BUFFER]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices*sizeof(GLuint), indices, GL_STATIC_DRAW);
	}

	//TODO: I need to add a property to each vertex to denote anchor

	glBindVertexArray(0);
}
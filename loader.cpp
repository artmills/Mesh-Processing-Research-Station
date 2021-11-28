#include "loader.hpp"

// get a RawModel from a list of Vertices.
void Loader::PrepareMesh(MeshComponent& mesh)
{
	// get a VAO ID and bind it:
	uint vaoID;
	InitializeVAO(vaoID);
	mesh.setVAO(vaoID);

	// bind the triangles buffer:
	AttributeList_Triangles(mesh.getTriangles());

	// store vertex data:
	AttributeList_StoreData(mesh.getVertices());

	// unbind the VAO:
	UnbindVAO();
}

// pass data to GPU:

void Loader::InitializeVAO(uint& vaoID)
{
	// create a VAO ID handle, add it to the VAO list, and bind it.
	glGenVertexArrays(1, &vaoID);
	glBindVertexArray(vaoID);
}
void Loader::UnbindVAO()
{
	glBindVertexArray(0);
}

void Loader::AttributeList_Triangles(std::vector<uint>& triangles)
{
	// create a VBO ID handle, add it to the list, and bind the data.
	uint vboID;
	glGenBuffers(1, &vboID);

	// triangle index arrays are ELEMENT ARRAY BUFFERS.
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, triangles.size() * sizeof(uint), &triangles[0], GL_STATIC_DRAW);

}

void Loader::AttributeList_StoreData(std::vector<Vertex>& vertices) 
{
	uint vboID;
	glGenBuffers(1, &vboID);

	uint vertexSize = sizeof(Vertex);

	glBindBuffer(GL_ARRAY_BUFFER, vboID);
	glBufferData(GL_ARRAY_BUFFER, vertexSize * vertices.size(), &vertices[0], GL_STATIC_DRAW); // easily get a pointer from std::vector by referencing the 0th element.

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexSize, 0); // positions 3D.
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, vertexSize, (const void*)(3 * sizeof(float))); // colors 4D.
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, vertexSize, (const void*)(7 * sizeof(float))); // normals 3D.
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, vertexSize, (const void*)(10 * sizeof(float))); // textures 2D.
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, vertexSize, (const void*)(12 * sizeof(float))); // barycentric 3D.
}

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
	mesh.setVBO(AttributeList_StoreData(mesh.getVertices()));

	// unbind the VAO:
	UnbindVAO();
}

// get a RawModel from a list of Vertices.
void Loader::PrepareCurve(CurveComponent& curve)
{
	// get a VAO ID and bind it:
	uint vaoID;
	InitializeVAO(vaoID);
	curve.setVAO(vaoID);

	// store vertex data:
	curve.setVBO(AttributeList_StoreData(curve.getVertices()));

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

void Loader::UpdateHighlight(uint vbo, uint v0, glm::vec4 color)
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	uint vertexSize = sizeof(Vertex);
	uint bytesToHighlightColor = 15 * sizeof(float);
	uint highlightColorSize = 4 * sizeof(float);
	std::vector<float> data = {color.r, color.g, color.b, color.a };

	// First vertex:
	glBufferSubData(GL_ARRAY_BUFFER, v0 * vertexSize + bytesToHighlightColor, highlightColorSize, &data[0]);
}
void Loader::UpdateHighlight(uint vbo, uint v0, uint v1, uint v2, glm::vec4 color)
{
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	uint vertexSize = sizeof(Vertex);
	uint bytesToHighlightColor = 15 * sizeof(float);
	uint highlightColorSize = 4 * sizeof(float);
	std::vector<float> data = {color.r, color.g, color.b, color.a };

	// First vertex:
	glBufferSubData(GL_ARRAY_BUFFER, v0 * vertexSize + bytesToHighlightColor, highlightColorSize, &data[0]);
	
	// Second vertex:
	glBufferSubData(GL_ARRAY_BUFFER, v1 * vertexSize + bytesToHighlightColor, highlightColorSize, &data[0]);
	
	// Third vertex:
	glBufferSubData(GL_ARRAY_BUFFER, v2 * vertexSize + bytesToHighlightColor, highlightColorSize, &data[0]);
}

uint Loader::AttributeList_StoreData(std::vector<LineVertex>& vertices) 
{
	uint vboID;
	glGenBuffers(1, &vboID);

	uint vertexSize = sizeof(LineVertex);

	glBindBuffer(GL_ARRAY_BUFFER, vboID);
	glBufferData(GL_ARRAY_BUFFER, vertexSize * vertices.size(), &vertices[0], GL_STATIC_DRAW); // easily get a pointer from std::vector by referencing the 0th element.

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexSize, 0); // positions 3D.
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, vertexSize, (const void*)(3 * sizeof(float))); // colors 4D.

	return vboID;
}
uint Loader::AttributeList_StoreData(std::vector<Vertex>& vertices) 
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
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, vertexSize, (const void*)(15 * sizeof(float))); // highlight color 4D.

	return vboID;
}

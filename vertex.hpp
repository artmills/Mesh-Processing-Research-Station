#pragma once

#include <iostream>
#include "glm/glm.hpp"

/** The primitive components of meshes.
 *
 * The current implementation contains these fields:
 *
 * 1) 3D Position.
 * 2) 4D Color (RGBA).
 * 3) 3D Normals.
 * 4) 2D Textures.
 * 5) 3D Barycentric coordinates.
 * 6) 4D Highlight color.
 * *
 * These vertices are arranged into triangles to form meshes. */
struct Vertex
{
public:

	/** Blank vertex. */
	Vertex();


	/** Specify the positions and colors for this vertex. */
	Vertex(float _x, float _y, float _z, float _r, float _g, float _b, float _a);
	Vertex(float _x, float _y, float _z, glm::vec4 color);


	/** Set the position of this vertex to the given position (x, y, z). */
	void setPosition(float x, float y, float z);


	/** Set the color of this vertex to the given RGBA color (r, g, b, a). */
	void setColor(float r, float g, float b, float a);
	void setColor(glm::vec4 color);

	/** Set the normal vector of this vertex. **/
	void setNormal(float nx, float ny, float nz);

	/** Set the texture coordinates of the vertex. **/
	void setTexture(float s, float t);

	/** Set the barycentric coordinate of this vertex. **/
	void setBarycentricCoordinate(glm::vec3 b);

	/** Set the highlight color of this vertex. **/
	void setHighlightColor(glm::vec4 color);

	/** Get the position of this vertex. **/
	glm::vec3 getPosition();

	/** Get the normal vector of this vertex. **/
	glm::vec3 getNormal();

	/* 3D Position. */
	float x, y, z;

	/* 4D RGBA Color. */
	float r, g, b, a;

	/* 3D Normal Vector. */
	float nx, ny, nz;

	/* 2D Texture. */
	float s, t;

	/* 3D Barycentric Coordinate. */
	float b1, b2, b3;

	/* 4D Highlight Color. */
	float h1, h2, h3, h4;

	/** Debug print. This really shouldn't be here. */
	void PrintPosition();
};

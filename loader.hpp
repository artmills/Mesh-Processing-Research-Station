#pragma once

#include <GL/glew.h>

#include <vector>
#include <iostream>

#include "utilities.hpp"
#include "meshcomponent.hpp"

/** Static class that loads model data into the GPU and returns a RawModel object with its VAO ID. 
 * 
 * The most important method is PrepareMesh() which passes a mesh component to the GPU. */
class Loader
{
public:

	/** Translate the data in the given mesh component into VAO data.
	 *
	 * This assumes the mesh is formatted as TRIANGLES.
	 * 
	 * When an entity is created with a mesh, this function should be called on the mesh to register it to the GPU. */
	static void PrepareMesh(MeshComponent& mesh);

private:

	// pass data to GPU:

	/** Given an empty uint, get a VAO ID from OpenGL and activate it. 
	 * 
	 * While this VAO ID is active, data can be assigned to it in the GPU.*/
	static void InitializeVAO(uint& vaoID);

	/** Deactivate the current VAO so that a new one can be registered. */
	static void UnbindVAO();

	/** WHILE A VAO IS ACTIVE: 
	 * 
	 * Generate a VBO consisting of the arrangement of triangles.
	 * The triangles are stored as an ordered list of uints indicating how the vertices are stitched together. */
	static void AttributeList_Triangles(std::vector<uint>& triangles);

	/* WHILE A VAO IS ACTIVE:
	 *
	 * Generate a VBO for vertex information.
	 * 
	 * THIS METHOD NEEDS TO BE UPDATED whenever more information is added to the Vertex struct.
	 * Currently, this method accounts for the following:
	 * 1) Positions 3D.
	 * 2) Colors 4D.
	 * */
	static void AttributeList_StoreData(std::vector<Vertex>& vertices);


	Loader();
	~Loader();

};

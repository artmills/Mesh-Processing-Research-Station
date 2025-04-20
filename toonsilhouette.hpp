#pragma once

#include "glm/glm.hpp"
#include <iostream>

#include "utilities.hpp"
#include "shaderprogram.hpp"

/** Part 1 of the implementation of a toon shader.
 * This shader intends to render the silhouette of a given mesh using a fancy trick.
 * The vertices of the mesh are first translated a small amount (uTranslate) along the vertex normal.
 * Next, the entire mesh is completely colored black.
 * Finally, the mesh is rendered with the FRONT FACES culled.
 *
 * When the normal mesh is rendered as well, the result will have the silhouette rendered alongside the normal mesh.
 *
 * This shader accounts for the following attributes:
 *
 * 1) vec3 vPosition
 * 3) vec3 vNormal
 *
 * and the following uniforms:
 *
 * 1) mat4 uProjectionMatrix
 * 2) mat4 uViewMatrix
 * 3) mat4 uTransformMatrix
 * 4) float uTranslate
 */
class ToonSilhouette : public ShaderProgram
{
public:

	/** Initialize the location of the shader source file. */
	ToonSilhouette();
	~ToonSilhouette();


	/** Call PrepareShader(). */
	void Initialize();


	/** Load the given projection matrix as a uniform. */
	void LoadProjectionMatrix(glm::mat4& projection);

	/** Load the given view matrix as a uniform. */
	void LoadViewMatrix(glm::mat4& view);

	/** Load the given transform matrix as a uniform. */
	void LoadTransformMatrix(glm::mat4& transform);

	/** Load the translation factor as a uniform. */
	void LoadTranslate(float& translate);

private:

	/** Path to shader source file. */
	static std::string shaderFile;


	/** ID of the MVP matrix given by GetUniformLocation(mvp). */
	uint locationProjectionMatrix;
	uint locationViewMatrix;
	uint locationTransformMatrix;
	uint locationTranslate;

	/** Debug print method. This really shouldn't be here. */
	void PrintRowMajor(glm::mat4& matrix);

protected:

	/** Bind the following attributes: 
	 * 
	 * 1) 0 -> vPosition.
	 * 3) 2 -> vNormal.
	 */
	void BindAttributes();


	/** Get all uniform locations from the GPU:
	 *
	 * 1) Projection matrix.
	 * 2) View matrix.
	 * 3) Model transform matrix.
	 */
	void GetAllUniformLocations();

};


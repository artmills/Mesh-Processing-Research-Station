#pragma once

#include "glm/glm.hpp"
#include <iostream>

#include "utilities.hpp"
#include "shaderprogram.hpp"

/** Implementation of a shader program for rendering curves.
 *
 * This shader accounts for the following attributes:
 *
 * 1) vec3 vPosition
 * 2) vec4 vColor
 *
 * and the following uniforms:
 *
 * 1) mat4 uProjectionMatrix
 * 2) mat4 uViewMatrix
 * 3) mat4 uTransformMatrix
 */
class LineShader : public ShaderProgram
{
public:

	/** Initialize the location of the shader source file. */
	LineShader();
	~LineShader();

	/** Call PrepareShader(). */
	void Initialize();


	/** Load the given projection matrix as a uniform. */
	void LoadProjectionMatrix(glm::mat4& projection);

	/** Load the given view matrix as a uniform. */
	void LoadViewMatrix(glm::mat4& view);

	/** Load the given transform matrix as a uniform. */
	void LoadTransformMatrix(glm::mat4& transform);


private:

	/** Path to shader source file. */
	static std::string shaderFile;


	/** ID of the MVP matrix given by GetUniformLocation(mvp). */
	uint locationProjectionMatrix;
	uint locationViewMatrix;
	uint locationTransformMatrix;


	/** Debug print method. This really shouldn't be here. */
	void PrintRowMajor(glm::mat4& matrix);

protected:

	/** Bind the following attributes: 
	 * 
	 * 1) 0 -> vPosition.
	 * 2) 1 -> vColor.
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


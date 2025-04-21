#pragma once

#include "glm/glm.hpp"
#include <iostream>

#include "utilities.hpp"
#include "shaderprogram.hpp"

/** Implementation of depth peeling.
 *
 * This shader accounts for the following attributes:
 *
 * 1) vec3 vPosition
 *
 * and the following uniforms:
 *
 * 1) mat4 uProjectionMatrix
 * 2) mat4 uViewMatrix
 * 3) mat4 uTransformMatrix
 * 4) sampler2D uTexture
 * 5, 6, 7, 8) uWidth, uHeight, uNear, uFar
 *
 */
class PeelShader : public ShaderProgram
{
public:

	/** Initialize the location of the shader source file. */
	PeelShader();
	~PeelShader();


	/** Call PrepareShader(). */
	void Initialize();


	/** Load the given projection matrix as a uniform. */
	void LoadProjectionMatrix(glm::mat4& projection);

	/** Load the given view matrix as a uniform. */
	void LoadViewMatrix(glm::mat4& view);

	/** Load the given transform matrix as a uniform. */
	void LoadTransformMatrix(glm::mat4& transform);

	/** Load projection information. */
	void LoadProjection(int width, int height, float near, float far);

	/** Load texture. */
	void LoadTexture(int unit);


private:

	/** Path to shader source file. */
	static std::string shaderFile;


	/** ID of the MVP matrix given by GetUniformLocation(mvp). */
	uint locationProjectionMatrix;
	uint locationViewMatrix;
	uint locationTransformMatrix;

	/** Projection information. */
	uint locationWidth;
	uint locationHeight;
	uint locationNear;
	uint locationFar;

	/** ID of the texture-related uniforms. */
	uint locationTexture;

	/** Debug print method. This really shouldn't be here. */
	void PrintRowMajor(glm::mat4& matrix);

protected:

	/** Bind the following attributes: 
	 * 
	 * 1) 0 -> vPosition.
	 * 2) 1 -> vColor.
	 * 3) 2 -> vNormal.
	 */
	void BindAttributes();


	/** Get all uniform locations from the GPU:
	 *
	 * 1) Projection matrix.
	 * 2) View matrix.
	 * 3) Model transform matrix.
	 * 4) All five lighting-related uniforms.
	 */
	void GetAllUniformLocations();

};


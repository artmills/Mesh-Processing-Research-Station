#pragma once

#include "glm/glm.hpp"
#include <iostream>

#include "utilities.hpp"
#include "shaderprogram.hpp"

/** Part 2 of the implementation of toon shading.
 * For best results, run part 1 first while culling the front faces of the mesh: see toonsilhouette.hpp.
 * Get an intensity value similar to the diffuse lighting calculation.
 * But use the intensity value in [0, 1] to pick between a discrete range of colors.
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
 * 4) float uAmbient, uDiffuse, uSpecular
 * 5) float uShininess
 * 6) float uSpecularColor
 * 7) vec3 uLightPosition
 * 8) vec3 uEyePosition
 */
class ToonShader : public ShaderProgram
{
public:

	/** Initialize the location of the shader source file. */
	ToonShader();
	~ToonShader();


	/** Call PrepareShader(). */
	void Initialize();


	/** Load the given projection matrix as a uniform. */
	void LoadProjectionMatrix(glm::mat4& projection);

	/** Load the given view matrix as a uniform. */
	void LoadViewMatrix(glm::mat4& view);

	/** Load the given transform matrix as a uniform. */
	void LoadTransformMatrix(glm::mat4& transform);

	/** Load lighting-related uniforms. */
	void LoadLighting(float ambient, float diffuse, float specular,
					  float shininess, 
					  glm::vec3 specularColor,
					  glm::vec3 lightPosition,
					  glm::vec3 eyePosition);


private:

	/** Path to shader source file. */
	static std::string shaderFile;


	/** ID of the MVP matrix given by GetUniformLocation(mvp). */
	uint locationProjectionMatrix;
	uint locationViewMatrix;
	uint locationTransformMatrix;

	/** ID of the lighting-related uniforms. */
	uint locationAmbient;
	uint locationDiffuse;
	uint locationSpecular;
	uint locationShininess;
	uint locationSpecularColor;
	uint locationLightPosition;
	uint locationEyePosition;

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
	 * 4) All five lighting-related uniforms.
	 */
	void GetAllUniformLocations();

};


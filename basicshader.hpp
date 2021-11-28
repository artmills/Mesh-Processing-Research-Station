#pragma once

#include "glm/glm.hpp"
#include <iostream>

#include "utilities.hpp"
#include "shaderprogram.hpp"

/** Implementation of a basic shader program.
 *
 * This shader accounts for the following attributes:
 *
 * 1) vec3 vPosition
 * 2) vec4 vColor
 * 3) vec3 vNormal
 * 4) vec2 vTexture
 * 5) vec3 vBarycentric
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
 * 10) mat4 uLightViewMatrix
 * 11) mat4 uLightPerspectiveMatrix
 */
class BasicShader : public ShaderProgram
{
public:

	/** Initialize the location of the shader source file. */
	BasicShader();
	~BasicShader();


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

	/** Load shadow-related uniforms. */
	void LoadShadows(glm::mat4 lightView, glm::mat4 lightPerspective, int shadowMap);

	/** Load texture. */
	void LoadTexture(int unit);


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

	/** ID of the shadow-related uniforms. */
	uint locationLightView;
	uint locationLightPerspective;
	uint locationShadowMap;

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
	 * 4) 3 -> vTexture.
	 * 5) 4 -> vBarycentric.
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


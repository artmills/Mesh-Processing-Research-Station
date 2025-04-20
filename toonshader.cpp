#include "toonshader.hpp"

// get the shader file for this shader and handle the OpenGL functions to prepare it.
ToonShader::ToonShader() : ShaderProgram()
{
	// despite how header files work, referencing a file as a string like this
	// is done relative to the location of the makefile, I guess.
	shaderFile = "toon.shader";
}
ToonShader::~ToonShader() {}

void ToonShader::Initialize()
{
	PrepareShader(shaderFile);
}

// bind position, color, etc.
void ToonShader::BindAttributes()
{
	BindAttribute(0, "vPosition");
	BindAttribute(2, "vNormal");
}

// get uniform locations so uniforms can be bound to the correct shader variables.
void ToonShader::GetAllUniformLocations()
{
	locationProjectionMatrix = GetUniformLocation("uProjectionMatrix");
	locationViewMatrix = GetUniformLocation("uViewMatrix");
	locationTransformMatrix = GetUniformLocation("uTransformMatrix");


	locationAmbient = GetUniformLocation("uAmbient");
	locationDiffuse = GetUniformLocation("uDiffuse");
	locationSpecular = GetUniformLocation("uSpecular");
	locationShininess = GetUniformLocation("uShininess");
	locationSpecularColor = GetUniformLocation("uSpecularColor");

	locationLightPosition = GetUniformLocation("uLightPosition");
	locationEyePosition = GetUniformLocation("uEyePosition");

	// Texture buffer object:
	locationCurvatureData = GetUniformLocation("uCurvatureData");
	glUniform1i(locationCurvatureData, 1);
}

// only need one matrix: modelViewProjection = model * view * projection.
void ToonShader::LoadProjectionMatrix(glm::mat4& projection)
{
	//PrintRowMajor(mvp);
	LoadUniform(locationProjectionMatrix, projection);
}

// only need one matrix: modelViewProjection = model * view * projection.
void ToonShader::LoadViewMatrix(glm::mat4& view)
{
	//PrintRowMajor(mvp);
	LoadUniform(locationViewMatrix, view);
}

void ToonShader::LoadTransformMatrix(glm::mat4& transform)
{
	//PrintRowMajor(mvp);
	LoadUniform(locationTransformMatrix, transform);
}

void ToonShader::LoadLighting(float ambient, float diffuse, float specular,
				  float shininess, 
				  glm::vec3 specularColor,
				  glm::vec3 lightPosition,
				  glm::vec3 eyePosition)
{
	LoadUniform(locationAmbient, ambient);
	LoadUniform(locationDiffuse, diffuse);
	LoadUniform(locationSpecular, specular);
	LoadUniform(locationShininess, shininess);
	LoadUniform(locationSpecularColor, specularColor);

	LoadUniform(locationLightPosition, lightPosition);
	LoadUniform(locationEyePosition, eyePosition);
}


std::string ToonShader::shaderFile;


void ToonShader::PrintRowMajor(glm::mat4& matrix)
{
	for (uint i = 0; i < 4; i++)
	{
		for (uint j = 0; j < 4; j++)
		{
			std::cout << matrix[i][j] << " ";
		}
		std::cout << std::endl;
	}
}

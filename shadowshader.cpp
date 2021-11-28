#include "shadowshader.hpp"

// get the shader file for this shader and handle the OpenGL functions to prepare it.
ShadowShader::ShadowShader() : ShaderProgram()
{
	// despite how header files work, referencing a file as a string like this
	// is done relative to the location of the makefile, I guess.
	shaderFile = "shadow.shader";
}
ShadowShader::~ShadowShader() {}

void ShadowShader::Initialize()
{
	PrepareShader(shaderFile);
}

// bind position, color, etc.
void ShadowShader::BindAttributes()
{
	BindAttribute(0, "vPosition");
}

// get uniform locations so uniforms can be bound to the correct shader variables.
void ShadowShader::GetAllUniformLocations()
{
	locationProjectionMatrix = GetUniformLocation("uLightPerspectiveMatrix");
	locationViewMatrix = GetUniformLocation("uLightViewMatrix");
	locationTransformMatrix = GetUniformLocation("uTransformMatrix");
}

// only need one matrix: modelViewProjection = model * view * projection.
void ShadowShader::LoadProjectionMatrix(glm::mat4& projection)
{
	//PrintRowMajor(mvp);
	LoadUniform(locationProjectionMatrix, projection);
}

// only need one matrix: modelViewProjection = model * view * projection.
void ShadowShader::LoadViewMatrix(glm::mat4& view)
{
	//PrintRowMajor(mvp);
	LoadUniform(locationViewMatrix, view);
}

void ShadowShader::LoadTransformMatrix(glm::mat4& transform)
{
	//PrintRowMajor(mvp);
	LoadUniform(locationTransformMatrix, transform);
}


std::string ShadowShader::shaderFile;


void ShadowShader::PrintRowMajor(glm::mat4& matrix)
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

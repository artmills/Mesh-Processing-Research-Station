#include "peelshader.hpp"

// get the shader file for this shader and handle the OpenGL functions to prepare it.
PeelShader::PeelShader() : ShaderProgram()
{
	// despite how header files work, referencing a file as a string like this
	// is done relative to the location of the makefile, I guess.
	shaderFile = "peel.shader";
}
PeelShader::~PeelShader() {}

void PeelShader::Initialize()
{
	PrepareShader(shaderFile);
}

// bind position, color, etc.
void PeelShader::BindAttributes()
{
	BindAttribute(0, "vPosition");
}

// get uniform locations so uniforms can be bound to the correct shader variables.
void PeelShader::GetAllUniformLocations()
{
	locationProjectionMatrix = GetUniformLocation("uProjectionMatrix");
	locationViewMatrix = GetUniformLocation("uViewMatrix");
	locationTransformMatrix = GetUniformLocation("uTransformMatrix");

	locationTexture = GetUniformLocation("uTexture");
	locationWidth = GetUniformLocation("uWidth");
	locationHeight = GetUniformLocation("uHeight");
	locationNear = GetUniformLocation("uNear");
	locationFar = GetUniformLocation("uFar");
}

// only need one matrix: modelViewProjection = model * view * projection.
void PeelShader::LoadProjectionMatrix(glm::mat4& projection)
{
	//PrintRowMajor(mvp);
	LoadUniform(locationProjectionMatrix, projection);
}

// only need one matrix: modelViewProjection = model * view * projection.
void PeelShader::LoadViewMatrix(glm::mat4& view)
{
	//PrintRowMajor(mvp);
	LoadUniform(locationViewMatrix, view);
}

void PeelShader::LoadTransformMatrix(glm::mat4& transform)
{
	//PrintRowMajor(mvp);
	LoadUniform(locationTransformMatrix, transform);
}

void PeelShader::LoadProjection(int width, int height, float near, float far)
{
	LoadUniform(locationWidth, width);
	LoadUniform(locationHeight, height);
	LoadUniform(locationNear, near);
	LoadUniform(locationFar, far);
}

void PeelShader::LoadTexture(int unit)
{
	LoadUniform(locationTexture, unit);
}

std::string PeelShader::shaderFile;


void PeelShader::PrintRowMajor(glm::mat4& matrix)
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

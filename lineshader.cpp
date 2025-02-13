#include "lineshader.hpp"

// get the shader file for this shader and handle the OpenGL functions to prepare it.
LineShader::LineShader() : ShaderProgram()
{
	// despite how header files work, referencing a file as a string like this
	// is done relative to the location of the makefile, I guess.
	shaderFile = "line.shader";
}
LineShader::~LineShader() {}

void LineShader::Initialize()
{
	PrepareShader(shaderFile);
}

// bind position and color.
void LineShader::BindAttributes()
{
	BindAttribute(0, "vPosition");
	BindAttribute(1, "vColor");
}

// get uniform locations so uniforms can be bound to the correct shader variables.
void LineShader::GetAllUniformLocations()
{
	locationProjectionMatrix = GetUniformLocation("uProjectionMatrix");
	locationViewMatrix = GetUniformLocation("uViewMatrix");
	locationTransformMatrix = GetUniformLocation("uTransformMatrix");


}

// only need one matrix: modelViewProjection = model * view * projection.
void LineShader::LoadProjectionMatrix(glm::mat4& projection)
{
	//PrintRowMajor(mvp);
	LoadUniform(locationProjectionMatrix, projection);
}

// only need one matrix: modelViewProjection = model * view * projection.
void LineShader::LoadViewMatrix(glm::mat4& view)
{
	//PrintRowMajor(mvp);
	LoadUniform(locationViewMatrix, view);
}

void LineShader::LoadTransformMatrix(glm::mat4& transform)
{
	//PrintRowMajor(mvp);
	LoadUniform(locationTransformMatrix, transform);
}


std::string LineShader::shaderFile;


void LineShader::PrintRowMajor(glm::mat4& matrix)
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

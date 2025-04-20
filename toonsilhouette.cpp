#include "toonsilhouette.hpp"

// get the shader file for this shader and handle the OpenGL functions to prepare it.
ToonSilhouette::ToonSilhouette() : ShaderProgram()
{
	// despite how header files work, referencing a file as a string like this
	// is done relative to the location of the makefile, I guess.
	shaderFile = "toonsilhouette.shader";
}
ToonSilhouette::~ToonSilhouette() {}

void ToonSilhouette::Initialize()
{
	PrepareShader(shaderFile);
}

// bind position, color, etc.
void ToonSilhouette::BindAttributes()
{
	BindAttribute(0, "vPosition");
	BindAttribute(2, "vNormal");
}

// get uniform locations so uniforms can be bound to the correct shader variables.
void ToonSilhouette::GetAllUniformLocations()
{
	locationProjectionMatrix = GetUniformLocation("uProjectionMatrix");
	locationViewMatrix = GetUniformLocation("uViewMatrix");
	locationTransformMatrix = GetUniformLocation("uTransformMatrix");

	locationTranslate = GetUniformLocation("uTranslate");
}

// only need one matrix: modelViewProjection = model * view * projection.
void ToonSilhouette::LoadProjectionMatrix(glm::mat4& projection)
{
	//PrintRowMajor(mvp);
	LoadUniform(locationProjectionMatrix, projection);
}

// only need one matrix: modelViewProjection = model * view * projection.
void ToonSilhouette::LoadViewMatrix(glm::mat4& view)
{
	//PrintRowMajor(mvp);
	LoadUniform(locationViewMatrix, view);
}

void ToonSilhouette::LoadTransformMatrix(glm::mat4& transform)
{
	//PrintRowMajor(mvp);
	LoadUniform(locationTransformMatrix, transform);
}

void ToonSilhouette::LoadTranslate(float& translate)
{
	LoadUniform(locationTranslate, translate);
}


std::string ToonSilhouette::shaderFile;


void ToonSilhouette::PrintRowMajor(glm::mat4& matrix)
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

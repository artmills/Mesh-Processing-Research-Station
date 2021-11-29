#include "basicshader.hpp"

// get the shader file for this shader and handle the OpenGL functions to prepare it.
BasicShader::BasicShader() : ShaderProgram()
{
	// despite how header files work, referencing a file as a string like this
	// is done relative to the location of the makefile, I guess.
	shaderFile = "basic.shader";
}
BasicShader::~BasicShader() {}

void BasicShader::Initialize()
{
	PrepareShader(shaderFile);
}

// bind position, color, etc.
void BasicShader::BindAttributes()
{
	BindAttribute(0, "vPosition");
	BindAttribute(1, "vColor");
	BindAttribute(2, "vNormal");
	BindAttribute(3, "vTexture");
	BindAttribute(4, "vBarycentric");
	BindAttribute(5, "vHighlight");
}

// get uniform locations so uniforms can be bound to the correct shader variables.
void BasicShader::GetAllUniformLocations()
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

	locationLightView = GetUniformLocation("uLightViewMatrix");
	locationLightPerspective = GetUniformLocation("uLightPerspectiveMatrix");
	locationShadowMap = GetUniformLocation("uShadowMap");
	locationTexture = GetUniformLocation("uTexture");

	locationWireframe = GetUniformLocation("uWireframe");
}

// only need one matrix: modelViewProjection = model * view * projection.
void BasicShader::LoadProjectionMatrix(glm::mat4& projection)
{
	//PrintRowMajor(mvp);
	LoadUniform(locationProjectionMatrix, projection);
}

// only need one matrix: modelViewProjection = model * view * projection.
void BasicShader::LoadViewMatrix(glm::mat4& view)
{
	//PrintRowMajor(mvp);
	LoadUniform(locationViewMatrix, view);
}

void BasicShader::LoadTransformMatrix(glm::mat4& transform)
{
	//PrintRowMajor(mvp);
	LoadUniform(locationTransformMatrix, transform);
}

void BasicShader::LoadLighting(float ambient, float diffuse, float specular,
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

void BasicShader::LoadShadows(glm::mat4 lightView, glm::mat4 lightPerspective, int shadowMap)
{
	LoadUniform(locationLightView, lightView);
	LoadUniform(locationLightPerspective, lightPerspective);
	LoadUniform(locationShadowMap, shadowMap);
}

void BasicShader::LoadTexture(int unit)
{
	LoadUniform(locationTexture, unit);
}

void BasicShader::LoadWireframe(bool enableWireframe)
{
	int wireframe = enableWireframe ? 1 : 0;
	LoadUniform(locationWireframe, wireframe);
}


std::string BasicShader::shaderFile;


void BasicShader::PrintRowMajor(glm::mat4& matrix)
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

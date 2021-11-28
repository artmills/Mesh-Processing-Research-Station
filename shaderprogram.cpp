#include "shaderprogram.hpp"

ShaderProgram::ShaderProgram() {}
ShaderProgram::~ShaderProgram() {}


// actual shader functionality.

void ShaderProgram::Start()
{
	glUseProgram(programID);
}
void ShaderProgram::Stop()
{
	glUseProgram(0);
}
void ShaderProgram::CleanUp()
{
	Stop();
	glDetachShader(programID, vertexShaderID);
	glDetachShader(programID, geometryShaderID);
	glDetachShader(programID, fragmentShaderID);
	glDeleteShader(vertexShaderID);
	glDeleteShader(geometryShaderID);
	glDeleteShader(fragmentShaderID);
	glDeleteProgram(programID);
}


void ShaderProgram::PrepareShader(const std::string& file)
{
	// parse the code in the given source file.
	ShaderProgramSource shaders = ParseShader(file);	

	// load up the two (or more) shaders to the GPU.
	LoadShader(vertexShaderID, ShaderType::vertex, shaders.vertexSource);
	LoadShader(fragmentShaderID, ShaderType::fragment, shaders.fragmentSource);
	LoadShader(geometryShaderID, ShaderType::geometry, shaders.geometrySource);

	// create the program and attach it.
	programID = glCreateProgram();
	glAttachShader(programID, vertexShaderID);
	glAttachShader(programID, fragmentShaderID);
	glAttachShader(programID, geometryShaderID);

	// bind attributes for the shader variables.
	BindAttributes();

	// link and validate.
	glLinkProgram(programID);
	glValidateProgram(programID);

	// evaluate uniform variables.
	GetAllUniformLocations();
}


// attributes and uniforms:

void ShaderProgram::BindAttribute(int attributeNumber, const std::string& variableName)
{
	glBindAttribLocation(programID, attributeNumber, variableName.c_str());
}

uint ShaderProgram::GetUniformLocation(const std::string& uniformName)
{
	return glGetUniformLocation(programID, uniformName.c_str());
}





void ShaderProgram::LoadShader(uint& shaderID, ShaderType shaderType, const std::string& text)
{
	// OpenGL uses c-style strings.
	const char* source = text.c_str();

	// convert between our ShaderType enum and the proper GL_ENUM.
	GLenum glShaderType;
	if (shaderType == vertex)
	{
		glShaderType = GL_VERTEX_SHADER;
	}
	else if (shaderType == geometry)
	{
		glShaderType = GL_GEOMETRY_SHADER;
	}
	else if (shaderType == fragment)
	{
		glShaderType = GL_FRAGMENT_SHADER;
	}
	else
	{
		// ERROR or forgot to add in a new shader type.
		glShaderType = -1;
		std::cout << "ERROR: Loading shader failed. " << std::endl;
	}
	
	// now that we have gotten the correct source, send it to OpenGL.
	// as long as the string isn't empty, such as if there is no geometry shader.
	if (source[0] != '\0')
	{
		shaderID = glCreateShader(glShaderType);
		glShaderSource(shaderID, 1, &source, nullptr);
	}
}

// TODO: Exception handling for files not existing, etc.
ShaderProgram::ShaderProgramSource ShaderProgram::ParseShader(const std::string& filePath)
{
	std::ifstream stream(filePath); // get input stream.
	std::string line;
	std::stringstream ss[3];
	ShaderType type = none;

	// go through the file one line at a time:
	while (getline(stream, line))
	{
		// a single file will contain both vertex and fragment shaders
		// identified by a "#shader" header.
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos)
			{
				type = vertex;
			}
			else if (line.find("fragment") != std::string::npos)
			{
				type = fragment;
			}
			else if (line.find("geometry") != std::string::npos)
			{
				type = geometry;
			}
		}
		else
		{
			// then this line is actual shader code. 
			ss[(int)type] << line << '\n';
		}
	}

	return {ss[0].str(), ss[1].str(), ss[2].str()};
}


// load uniforms to GPU:
void ShaderProgram::LoadUniform(uint location, glm::mat4& matrix)
{
	glUniformMatrix4fv(location, 1, GL_FALSE, &matrix[0][0]); // same as the easy way to get a pointer to an array's entries.
}
void ShaderProgram::LoadUniform(uint location, glm::vec4& v)
{
	glUniform4f(location, v.x, v.y, v.z, v.w);
}
void ShaderProgram::LoadUniform(uint location, glm::vec3& v)
{
	glUniform3f(location, v.x, v.y, v.z);
}
void ShaderProgram::LoadUniform(uint location, glm::vec2& v)
{
	glUniform2f(location, v.x, v.y);
}
void ShaderProgram::LoadUniform(uint location, float x)
{
	glUniform1f(location, x);
}
void ShaderProgram::LoadUniform(uint location, int n)
{
	glUniform1i(location, n);
}
















#pragma once

#include <GL/glew.h>
#include "glm/glm.hpp"

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>

#include "utilities.hpp"

// wrapper for basic shader functions. implement them by creating a new shader class
// that inherits from ShaderProgram.
/** Wrapper for basic shader functions.
 *
 * New shaders should implement this class. */
class ShaderProgram
{
public:

	ShaderProgram();
	~ShaderProgram();

	/** Given the path to a text file containing the shader code, load it into the GPU.
	 * The file path is relative to the makefile.
	 * 
	 * This method should be called when the shader is first initialized.
	 * A great place to do this would be in the Start function of the engine. */
	void PrepareShader(const std::string& file);

	/** When loading the shader into the GPU, we need to specify what type of shader it is. */
	enum ShaderType
	{
		none = -1, vertex = 0, fragment = 1, geometry = 2
	};

	/** A convenient way of combining the source code of all shader sources.
	 * 
	 * If this struct is updated to hold more than just vertex and fragment shaders, the ParseShader() method should also be updated. */
	struct ShaderProgramSource // just a way of returning both shader files in a single method.
	{
		std::string vertexSource;
		std::string fragmentSource;
		std::string geometrySource;
	};

	
	/** Activate the shader. 
	 *
	 * This should be called at the beginning of each Update() method. */
	void Start();

	/** Deactivate the shader. 
	 *
	 * This should be called at the end of each Update() method. */
	void Stop();

	/** Release all resources. */
	void CleanUp();


private:

	/** The ID of this particular shader in GPU. 
	 *
	 * This ID is the total sum of all the different shader "pieces" corresponding to this one.
	 * To add in more shader components, their IDs need to be tracked below. */
	uint programID;

	/** The ID of the vertex shader component. */
	uint vertexShaderID;

	/** The ID of the geometry shader component. */
	uint geometryShaderID;

	/** The ID of the fragment shader component. */
	uint fragmentShaderID;

	/** File I/O: load up the shader at the file path. 
	 *
	 * The shaderID should correspond to the shader type.
	 * For example, if the shader is the vertex shader, the shaderID should be m_vertexShaderID.
	 *
	 * ShaderType specifies the type of shader.
	 *
	 * The text argument is the filepath to the shader file.
	 * The path is relative to the location of the makefile. */
	static void LoadShader(uint& shaderID, ShaderType shaderType, const std::string& text);



	/** File I/O: parse the shader text file.
	 *
	 * This method takes in a text file and returns the actual shader code as a ShaderProgramSource object.
	 *
	 * WARNING: This method assumes that the vertex and fragment shaders are both in the same text file.
	 * That is, this function will parse the entire file and separate out the vertex and fragment shader code and put those together in a single ShaderProgramSource object.
	 * If new shader types are added, this function will need to be adjusted. */
	static ShaderProgramSource ParseShader(const std::string& textfile);

protected:

	/** Abstract method that will call the BindAttribute() method for each attribute that the specific shader requires. */
	virtual void BindAttributes() = 0;



	/** Initializes the "variable" in the shader code. 
	 * In order to use a variable such as vec3 position in shader code, that variable needs to be initialized.
	 * Call this method on each variable.
	 *
	 * attributeNumber refers to the ordering of the appearance of the variable in shader code, beginning with 0. 
	 *
	 * variableName is the name of that variable.
	 *
	 * Make sure that these values match what is written in the shader code!*/
	void BindAttribute(int attributeNumber, const std::string& variableName);


	/** This method is a reminder to initialize all uniform variables properly in each shader.
	 *
	 * A uniform is a variable that is applied globally to everything that is being drawn by the shader, such as the location of a camera or light source.
	 *
	 * Each shader will have its own set of uniforms.
	 * The IDs of those uniforms need to be initialized, which is handled in the GetUniformLocation() method. */
	virtual void GetAllUniformLocations() = 0;



	/** Ask OpenGL for an ID for the given uniform variable.
	 * This ID is the order of the uniform variable list in the shader code. */
	uint GetUniformLocation(const std::string& uniformName);



	/** Load the uniform variable at the given location.
	 * 
	 * This method should be overloaded for all kinds of data that will be sent to the GPU. */
	void LoadUniform(uint location, glm::mat4& matrix);
	void LoadUniform(uint location, glm::vec4& v);
	void LoadUniform(uint location, glm::vec2& v);
	void LoadUniform(uint location, glm::vec3& v);
	void LoadUniform(uint location, float x);
	void LoadUniform(uint location, int n);








};

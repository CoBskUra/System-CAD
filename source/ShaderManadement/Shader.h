#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H

#include<glad/glad.h>
#include<string>
#include<fstream>
#include<sstream>
#include<iostream>
#include<cerrno>
#include <vector>


struct shaderInput {
	shaderInput(const char* shaderPath, const char* shaderName,	GLenum shaderType)
		:shaderName{ shaderName }, shaderPath{ shaderPath }, shaderType{ shaderType } 
	{	}

	const char* shaderName;
	const char* shaderPath;
	GLenum shaderType;
};

class Shader
{
public:
	GLuint ID;
	Shader();

	Shader(const char* vertexFile, const char* fragmentFile);
	Shader(std::vector<shaderInput> shadersTypes);

	void Activate();
	void Delete();

private:
	void compileErrors(unsigned int shader, const char* type);
	GLuint CreateShader(const char* ShaderPath, const char* shaderName, GLenum ShaderType);
};

std::string get_file_contents(const char* filename);


#endif
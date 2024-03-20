#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H

#include<glad/glad.h>
#include<string>
#include<fstream>
#include<sstream>
#include<iostream>
#include<cerrno>


class Shader
{
public:
	GLuint ID;
	Shader();

	Shader(const char* vertexFile, const char* fragmentFile);
	void Activate();
	void Delete();

private:
	void compileErrors(unsigned int shader, const char* type);
	GLuint CreateFragmentShader(const char* fragmentShaderPath);
	GLuint CreateVertexShader(const char* fragmentShaderPath);

};

std::string get_file_contents(const char* filename);


#endif
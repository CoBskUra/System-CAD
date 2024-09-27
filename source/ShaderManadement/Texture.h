#pragma once

#include<glad/glad.h>

#include"Shader.h"

class Texture
{
public:
	GLuint ID;
	GLenum type;
	Texture(GLenum texType);

	// Assigns a texture unit to a texture
	void texUnit(Shader& shader, const char* uniform, GLuint unit);
	// Binds a texture
	void Bind();
	// Unbinds a texture
	void Unbind();
	// Deletes a texture
	void Delete();

	void Recreat();

	~Texture();
};
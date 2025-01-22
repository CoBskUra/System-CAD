#include "Texture.h"

Texture::Texture(GLenum texType) {
	type = texType;
	// Generates an OpenGL texture object
	glGenTextures(1, &ID);
	//// Assigns the texture to a Texture Unit
	//glActiveTexture(slot);
	//glBindTexture(texType, ID);
	// Unbinds the OpenGL Texture object so that it can't accidentally be modified
	glBindTexture(texType, 0);
}



void Texture::texUnit(Shader& shader, const char* uniform, GLuint unit)
{
	// Gets the location of the uniform
	GLuint texUni = glGetUniformLocation(shader.ID, uniform);
	// Shader needs to be activated before changing the value of a uniform
	shader.Activate();
	// Sets the value of the uniform
	glUniform1i(texUni, unit);
}

void Texture::Bind()
{
	glBindTexture(type, ID);
}

void Texture::Unbind()
{
	glBindTexture(type, 0);
}

void Texture::Delete()
{
	glDeleteTextures(1, &ID);
}

void Texture::Recreat()
{
	glDeleteTextures(1, &ID);
	glGenTextures(1, &ID);
	// Assigns the texture to a Texture Unit
	//glActiveTexture(slot);
}

Texture::~Texture()
{
	Delete();
}

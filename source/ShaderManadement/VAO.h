#ifndef VAO_CLASS_H
#define VAO_CLASS_H

#include<glad/glad.h>
#include"VBO.h"

class VAO
{
public:
	GLuint ID;

	VAO(){
		glGenVertexArrays(1, &ID);
	}

	void LinkAttrib(GLuint layout, GLuint numComponents, GLenum type, bool normalize, GLsizeiptr stride, void* offset)
	{
		glVertexAttribPointer(layout, numComponents, type, normalize, stride, offset);
		glEnableVertexAttribArray(layout);
	}

	void Bind(){
		glBindVertexArray(ID);
	}

	void Unbind(){
		glBindVertexArray(0);
	}

	void Delete(){
		glDeleteVertexArrays(1, &ID);
	}

	void Reactive() {
		glDeleteVertexArrays(1, &ID);
		glGenVertexArrays(1, &ID);
	}

	~VAO() {
		Delete();
	}
};

#endif
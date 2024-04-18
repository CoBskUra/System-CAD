#ifndef VBO_CLASS_H
#define VBO_CLASS_H

#include<glm/glm.hpp>
#include<glad/glad.h>
#include<vector>

class VBO
{
public:
	GLuint ID;

	template<typename T>
	VBO(std::vector<T>& vertices, int typeOfDataManagement)
	{
		glGenBuffers(1, &ID);
		glBindBuffer(GL_ARRAY_BUFFER, ID);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(T), vertices.data(), typeOfDataManagement);
	}

	void Bind() {
		glBindBuffer(GL_ARRAY_BUFFER, ID);
	}

	void Unbind() {
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void Delete() {
		glDeleteBuffers(1, &ID);
	}


	~VBO() {
		Delete();
	}
};

#endif
#pragma once
#include "shaderClass.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include <glm/gtc/type_ptr.hpp>

class kwadrat
{
public:
	Shader shader;
	VAO vao;
	glm::vec4 color;

	kwadrat(Shader shader, float z, float size) 
	{ 
		this->shader = shader;
		float vertices[] = {
			size*0.5f, size* 0.5f, -z,  // top right
			size*0.5f, size*-0.5f, z,  // bottom right
			size*-0.5f,size* -0.5f, -z,  // bottom left
			size*-0.5f,size*  0.5f, z   // top left 
		};
		unsigned int indices[] = {  // note that we start from 0!
			0, 1, 3,  // first Triangle
			1, 2, 3   // second Triangle
		};
		std::vector<float> vector(vertices, vertices + sizeof(vertices) / sizeof(float));
		std::vector <GLuint> ind(indices, indices + sizeof(indices) / sizeof(GLuint));

		vao.Bind();
		VBO vbo(vector, GL_STATIC_DRAW);
		EBO ebo(ind);

		vao.LinkAttrib(0, 3, GL_FLOAT, false, 3 * sizeof(float), 0);

		vao.Unbind(); vbo.Unbind(); ebo.Unbind();
	}

	void Draw() {
		//shader.Activate();
		vao.Bind();

		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "MODEL_MATRIX"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "CAM_MATRIX"), 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
		glUniform4f(glGetUniformLocation(shader.ID, "COLOR"), color.x, color.y, color.z, color.w);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		vao.Unbind();
	}
};


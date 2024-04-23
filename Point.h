#pragma once
#include "Figure.h"

class Point : public Figure {
public:
	Point(Shader* shader, const char* name) : Point(shader, "##Point", FigureType::Point)
	{
		CreatePoint();
		SetName(name);
	}

	Point(Shader* shader) : Point(shader, "##Point", FigureType::Point)
	{
		CreatePoint();
		SetName("Point");
	}

	void virtual Draw(GLFWwindow* window, const Camera& camera) {
		shader->Activate();
		vao.Bind();

		glUniformMatrix4fv(glGetUniformLocation(shader->ID, "MODEL_MATRIX"),
			1, GL_FALSE, glm::value_ptr(GetModelMatrix()));
		glUniform4f(glGetUniformLocation(shader->ID, "COLOR"), color.x, color.y, color.z, color.w);
		camera.SaveMatrixToShader(shader->ID);
		glDrawArrays(GL_POINTS, 0, 1);
		vao.Unbind();
	}

	bool virtual Inputs(GLFWwindow* window, const Camera& camera) {
		return false;
	}
protected:
	Point(Shader* shader, const char* uniqueName, FigureType type) : Figure(shader, uniqueName, type){

	}

private:
	void CreatePoint() {
		vao.Bind();
		std::vector<float> vs = {0, 0, 0};
		VBO vbo(vs, GL_STATIC_DRAW);

		vao.LinkAttrib(0, 3, GL_FLOAT, false, 3 * sizeof(float), 0);

		vao.Unbind(); vbo.Unbind();
	}
};
#pragma once
#include "Figure.h"
#include "StaticShaders.h"

class Point : public Figure {
public:
	Point( const char* name) : Point("##Point", FigureType::Point)
	{
		CreatePoint();
		SetName(name);
	}

	Point( ): Point("Point")
	{
	}

	void virtual Draw(GLFWwindow* window, const Camera& camera) {
		pointShader->Activate();
		vao_point.Bind();
		auto showColor = GetShowColor();
		glUniformMatrix4fv(glGetUniformLocation(pointShader->ID, "MODEL_MATRIX"),
			1, GL_FALSE, glm::value_ptr(GetModelMatrix()));
		glUniform4f(glGetUniformLocation(pointShader->ID, "COLOR"), showColor.x, showColor.y, showColor.z, showColor.w);
		camera.SaveMatrixToShader(pointShader->ID);
		glDrawArrays(GL_POINTS, 0, 1);
		vao_point.Unbind();
	}

	bool virtual Inputs(GLFWwindow* window, const Camera& camera) {
		return false;
	}
protected:
	Point( const char* uniqueName, FigureType type) : Figure(uniqueName, type){
	}

private:
	VAO vao_point;
	Shader* pointShader = StaticShaders::GetPointerToPoint();

	void CreatePoint() {
		vao_point.Bind();
		std::vector<float> vs = {0, 0, 0};
		VBO vbo(vs, GL_STATIC_DRAW);

		vao_point.LinkAttrib(0, 3, GL_FLOAT, false, 3 * sizeof(float), 0);

		vao_point.Unbind(); vbo.Unbind();
	}
};
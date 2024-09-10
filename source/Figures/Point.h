#pragma once
#include "Figures/Figure.h"
#include "ShaderRefrence/StaticShaders.h"
#include "Models/Point.h"
#include "Models/SceneObject.h"

class Point : public Figure {
public:
	Point( const char* name) : Point( FigureType::Point)
	{
		CreatePoint();
		SetName(name);
	}

	Point( ): Point("Point")
	{
	}


	Point(MG1::Point sceneObject, int offsetId = 0) :Point(){
		this->transpose->SetObjectPosition(sceneObject.position.x, sceneObject.position.y, sceneObject.position.z);
		if (sceneObject.name != "")
			this->SetName(sceneObject.name.c_str());
		this->SetId(sceneObject.GetId() + offsetId);
	}

	MG1::Point Serialize(int idOffset) const {
		MG1::Point point{};
		point.SetId(GetId() - idOffset);
		point.name = name;
		point.position.x = GetPosition().x;
		point.position.y = GetPosition().y;
		point.position.z = GetPosition().z;
		return point;
	}

	void virtual Draw(GLFWwindow* window, const Camera& camera) {
		shader.Activate();
		vao_point.Bind();
		auto showColor = GetShowColor();
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "MODEL_MATRIX"),
			1, GL_FALSE, glm::value_ptr(GetModelMatrix()));
		glUniform4f(glGetUniformLocation(shader.ID, "COLOR"), showColor.x, showColor.y, showColor.z, showColor.w);
		camera.SaveMatrixToShader(shader.ID);
		glDrawArrays(GL_POINTS, 0, 1);
		vao_point.Unbind();
	}

	bool virtual Inputs(GLFWwindow* window, const Camera& camera) {
		return false;
	}
protected:
	Point( FigureType type) : Figure(type){
	}

private:
	VAO vao_point;
	const Shader& shader = StaticShaders::GetPoint();

	void CreatePoint() {
		vao_point.Bind();
		std::vector<float> vs = {0, 0, 0};
		VBO vbo(vs, GL_STATIC_DRAW);

		vao_point.LinkAttrib(0, 3, GL_FLOAT, false, 3 * sizeof(float), 0);

		vao_point.Unbind(); vbo.Unbind();
	}
};
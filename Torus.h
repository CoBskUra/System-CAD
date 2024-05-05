#ifndef Torus_CLASS_H
#define Torus_CLASS_H

#include"Shader.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include <glm/gtc/type_ptr.hpp>
#include "Constants.h"
#include "Figure3D.h"
#include "StaticShaders.h"

class Torus: public Figure3D
{
public:
	Torus( const char* name) : Figure3D("##Torus", FigureType::Torus)
	{
		CreateTorus();
		SetName(name);
	}

	Torus(): Torus("Torus")
	{
	}

	void virtual Draw(GLFWwindow* window, const Camera& camera) {
		torusShader->Activate();
		vao_torus.Bind();

		auto showColor = GetShowColor();

		glUniformMatrix4fv(glGetUniformLocation(torusShader->ID, "MODEL_MATRIX"),
			1, GL_FALSE, glm::value_ptr(GetModelMatrix()));
		glUniform4f(glGetUniformLocation(torusShader->ID, "COLOR"), showColor.x, showColor.y, showColor.z, showColor.w);
		camera.SaveMatrixToShader(torusShader->ID);

		glDrawElements(GL_LINE_STRIP, 2*segment1*segment2 + segment1 + segment2, GL_UNSIGNED_INT, 0);
		vao_torus.Unbind();
	}

	void virtual ActiveImGui() {
		ImGui::BeginGroup();
		{
			Figure3D::ActiveImGui();
			FigureSpecificImGui();
		}
		ImGui::EndGroup();
	}

	void virtual FigureSpecificImGui() {
		ImGui::BeginGroup();
		{
			ImGui::Text("Torus parameters");
			if (ImGui::InputInt(("segment 1" + GetUniqueName()).c_str(), &segment1) ||
				ImGui::InputInt(("segment 2" + GetUniqueName()).c_str(), &segment2) ||
				ImGui::DragFloat(("R" + GetUniqueName()).c_str(), &R, 0.1f, M_ESP) ||
				ImGui::DragFloat(("r" + GetUniqueName()).c_str(), &r, 0.1f, M_ESP))
				CreateTorus();
		}
		ImGui::EndGroup();
	}

private:
	Shader* torusShader = StaticShaders::GetPointerToTorus();
	VAO vao_torus;
	int segment1 = 10;
	int segment2 = 10;
	float R = 0.5f;
	float r = 0.2f;

	std::vector<float>  createTorusVertexBuffer() {
		std::vector<float> vertices;
		for (int i = 0; i < segment1; i++) {
			float theta1 = (2.0f * M_PI * i) / segment1;

			for (int j = 0; j < segment2; j++) {
				float phi1 = (2.0f * M_PI * j) / segment2;

				float x1 = (R + r * cos(theta1)) * cos(phi1);
				float y1 = (R + r * cos(theta1)) * sin(phi1);
				float z1 = r * sin(theta1);

				vertices.push_back(x1);
				vertices.push_back(y1);
				vertices.push_back(z1);

			}
		}
		return vertices;
	}

	std::vector<GLuint>  createTorusIndeces() {
		std::vector<GLuint> indeces;
		for (int i = 0; i < segment1; i++) {
			for (int j = 0; j < segment2; j++) {
				indeces.push_back(segment2 * i + j);
			}
			indeces.push_back(segment2* i );
		}

		for (int j = 0; j < segment2; j++) {
			for (int i = 0; i < segment1; i++) {
				indeces.push_back(segment2 * i + j);
			}
			indeces.push_back(j);
		}

		return indeces;
	}

	void CreateTorus() {
		vao_torus.Reactive();
		vao_torus.Bind();
		auto vs = createTorusVertexBuffer();
		auto ies = createTorusIndeces();
		VBO vbo(vs, GL_DYNAMIC_DRAW);
		EBO ebo(ies);

		vao_torus.LinkAttrib(0, 3, GL_FLOAT, false, 3 * sizeof(float), 0);

		vao_torus.Unbind(); vbo.Unbind(); ebo.Unbind();
	}
};
#endif
#ifndef Torus_CLASS_H
#define Torus_CLASS_H

#include "ShaderManadement/Shader.h"
#include "ShaderManadement/VAO.h"
#include "ShaderManadement/VBO.h"
#include "ShaderManadement/EBO.h"
#include <glm/gtc/type_ptr.hpp>
#include "Constants.h"
#include "Figures/Figure3D.h"
#include "ShaderRefrence/StaticShaders.h"
#include "Models/Torus.h"

class Torus: public Figure3D
{
public:
	Torus( const char* name) : Figure3D( FigureType::Torus)
	{
		CreateTorus();
		SetName(name);
	}

	Torus(): Torus("Torus")
	{
	}

	Torus(MG1::Torus tori, int offsetId):Torus() {
		this->transpose->SetObjectPosition(tori.position.x, tori.position.y, tori.position.z);
		Rotation rotation{};
		rotation.SetRotation_X(tori.rotation.x * M_PI / 180.0);
		rotation.SetRotation_Y(tori.rotation.y * M_PI / 180.0);
		rotation.SetRotation_Z(tori.rotation.z * M_PI / 180.0);
		this->scale.SetScale(tori.scale.x, tori.scale.y, tori.scale.z);
		this->quaternion.SetMainQuaternion((Quaternion)rotation.Get_R());
		if (tori.name != "")
			this->SetName(tori.name.c_str());
		this->SetId(tori.GetId() + offsetId);
	}

	MG1::Torus Serialize(int idOffset) const {
		MG1::Torus tori{};
		tori.SetId(GetId() - idOffset);
		tori.name = name;
		tori.position.x = GetPosition().x;
		tori.position.y = GetPosition().y;
		tori.position.z = GetPosition().z;
		auto angles = quaternion.GetEulerRadXYZ();
		tori.rotation.x = (angles.x / M_PI * 180.0);
		tori.rotation.y = (angles.y / M_PI * 180.0);
		tori.rotation.z = (angles.z / M_PI * 180.0);
		glm::vec3 scale = this->scale.GetScaleVec();
		tori.scale.x = scale.x;
		tori.scale.y = scale.y;
		tori.scale.z = scale.z;
		return tori;
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
			if (ImGui::InputInt("segment 1" , &segment1) ||
				ImGui::InputInt("segment 2", &segment2) ||
				ImGui::DragFloat("R" , &R, 0.1f, M_ESP) ||
				ImGui::DragFloat("r" , &r, 0.1f, M_ESP))
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
				float y1 = r * sin(theta1); 
				float z1 = (R + r * cos(theta1)) * sin(phi1);

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
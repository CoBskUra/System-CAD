#pragma once
#include "Quaternion.h"
#include <glm/ext/vector_float3.hpp>
#include <glm/fwd.hpp>
#include <ImGui/imgui.h>

class QuaternionRotationImGui : public Quaternion
{
	Quaternion setAbleQuaternion;
	float alfa = 0;
	glm::vec3 axis{ 0.0f };

	Quaternion* baseQuaternion = this;
	float mainQuaternion_alfa = 0;
	glm::vec3 mainQuaternion_axis{ 0.0f };
public:
	bool ActiveInterferes() {
		bool valueHasBeenUpdated = false;
		ImGui::BeginGroup();
		{
			ImGui::Text("Quaternion");
			ImGui::BeginGroup();
			{
				ImGui::InputFloat("Alfa##QuaternionRotationImGui_main", &mainQuaternion_alfa, 0.0f, 0.0f, "%.4f", ImGuiInputTextFlags_ReadOnly);
				ImGui::InputFloat("x##QuaternionRotationImGui_main", &mainQuaternion_axis.x, 0.0f, 0.0f, "%.4f", ImGuiInputTextFlags_ReadOnly);
				ImGui::InputFloat("y##QuaternionRotationImGui_main", &mainQuaternion_axis.y, 0.0f, 0.0f, "%.4f", ImGuiInputTextFlags_ReadOnly);
				ImGui::InputFloat("z##QuaternionRotationImGui_main", &mainQuaternion_axis.z, 0.0f, 0.0f, "%.4f", ImGuiInputTextFlags_ReadOnly);

			}
			ImGui::EndGroup();
			if (ImGui::DragFloat("Alfa##QuaternionRotationImGui", &alfa, 0.1f, -M_PI, M_PI, "%.3f", ImGuiSliderFlags_AlwaysClamp))
			{
				if (glm::dot(axis, axis) >= M_ESP) {
					axis = glm::normalize(axis);
					setAbleQuaternion.SetRotationAlong(alfa, axis);
					valueHasBeenUpdated = true;
				}
			}
			if (ImGui::DragFloat("x##QuaternionRotationImGui", &axis.x, 0.1f) ||
				ImGui::DragFloat("y##QuaternionRotationImGui", &axis.y, 0.1f) ||
				ImGui::DragFloat("z##QuaternionRotationImGui", &axis.z, 0.1f) )
			{
				SaveRotation();
				//valueHasBeenUpdated = true;
			}
			if (ImGui::Button("Override rotation##QuaternionRotationImGui"))
			{
				SetMainQuaternion(setAbleQuaternion);
				setAbleQuaternion = glm::vec4(1, 0, 0, 0);
				valueHasBeenUpdated = true;
			}
			ImGui::SameLine();
			if (ImGui::Button("Save rotation##QuaternionRotationImGui"))
			{
				SaveRotation();
				//valueHasBeenUpdated = true;
			}
		}
		ImGui::EndGroup();
		return valueHasBeenUpdated;
	}

	glm::mat4 virtual GetEulerRotation() const {
		return (setAbleQuaternion * (*baseQuaternion)).GetEulerRotation();
	}

	Quaternion virtual Invers() const {
		return (setAbleQuaternion * (*baseQuaternion) ).Invers();
	}

	void SetMainQuaternion(Quaternion q) {
		*baseQuaternion = (QuaternionRotationImGui)q;
		(*baseQuaternion).Normalize();
		auto alfa_axis = this->GetAngleRepresentation();
		mainQuaternion_alfa = alfa_axis.x;
		mainQuaternion_axis = glm::vec3(alfa_axis.y, alfa_axis.z, alfa_axis.w);
	}

	Quaternion Get() const {
		return setAbleQuaternion * (*baseQuaternion);
	}

	void SaveRotation()
	{
		alfa = 0;
		if(setAbleQuaternion != Quaternion(0, 0, 0, 0))
			SetMainQuaternion(setAbleQuaternion * (*baseQuaternion));
		setAbleQuaternion = glm::vec4(1, 0, 0, 0);
	}

	void Reset() {
		setAbleQuaternion = Quaternion();
		SetMainQuaternion(Quaternion());
	}

	QuaternionRotationImGui(Quaternion q): Quaternion(q) {
	}
	QuaternionRotationImGui():Quaternion() {};

	friend bool operator==(QuaternionRotationImGui const& q1, QuaternionRotationImGui const& q2);
	friend bool operator!=(QuaternionRotationImGui const& q1, QuaternionRotationImGui const& q2);
};


#pragma once
#include "AfirmatricTransformation.h"
#include <ImGui/imgui.h>

class AfirmationTransformation_ImGui: public AfirmatricTransformation
{
public:
	void ActiveInterferes() {
		ImGui::BeginGroup();
		{
			ImGui::Text("Rotation");
			if (ImGui::DragFloat("OX", &alfa_x, 0.1f, -M_PI, M_PI, "%.3f", ImGuiSliderFlags_AlwaysClamp))
				SetRotation_X(alfa_x);
			if (ImGui::DragFloat("OY", &alfa_y, 0.1f, -M_PI, M_PI, "%.3f", ImGuiSliderFlags_AlwaysClamp))
				SetRotation_Y(alfa_y);
			if (ImGui::DragFloat("OZ", &alfa_z, 0.1f, -M_PI, M_PI, "%.3f", ImGuiSliderFlags_AlwaysClamp))
				SetRotation_Z(alfa_z);
		}
		ImGui::EndGroup();

		ImGui::BeginGroup();
		{
			ImGui::Text("Translate");
			if (ImGui::DragFloat("x:", &ObjectPosition.x, 0.1f))
				SetObjectPosition(ObjectPosition);
			if (ImGui::DragFloat("y:", &ObjectPosition.y, 0.1f))
				SetObjectPosition(ObjectPosition);
			if (ImGui::DragFloat("z:", &ObjectPosition.z, 0.1f))
				SetObjectPosition(ObjectPosition);
		}
		ImGui::EndGroup();

		ImGui::BeginGroup();
		{
			ImGui::Text("Scale");
			if (ImGui::DragFloat("x", &ScaleVec.x, 0.1f, M_ESP))
				SetScale(ScaleVec);
			if (ImGui::DragFloat("y", &ScaleVec.y, 0.1f, M_ESP))
				SetScale(ScaleVec);
			if (ImGui::DragFloat("z", &ScaleVec.z, 0.1f, M_ESP))
				SetScale(ScaleVec);
		}
		ImGui::EndGroup();

	}
};


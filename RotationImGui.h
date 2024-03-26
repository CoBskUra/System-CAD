#pragma once
#include "Rotation.h"
#include <ImGui/imgui.h>
class RotationImGui :public Rotation {
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
	}
};
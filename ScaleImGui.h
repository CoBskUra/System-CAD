#pragma once
#include "Scale.h"
#include <ImGui/imgui.h>
#include "Constants.h"
class ScaleImGui : public Scale {
public:
	void ActiveInterferes() {
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
#pragma once
#pragma once
#include "transpose.h"
#include <ImGui/imgui.h>
#include "Constants.h"
class TransposeImGui : public Transpose {
public:
	void ActiveInterferes() {
		ImGui::BeginGroup();
		{
			glm::vec3 objectPositionCopy = ObjectPosition;
			ImGui::Text("Translate");
			if (ImGui::DragFloat("x:", &objectPositionCopy.x, 0.1f))
				SetObjectPosition(objectPositionCopy);
			if (ImGui::DragFloat("y:", &objectPositionCopy.y, 0.1f))
				SetObjectPosition(objectPositionCopy);
			if (ImGui::DragFloat("z:", &objectPositionCopy.z, 0.1f))
				SetObjectPosition(objectPositionCopy);
		}
		ImGui::EndGroup();
	}
};
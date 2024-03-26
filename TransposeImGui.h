#pragma once
#pragma once
#include "Transpose.h"
#include <ImGui/imgui.h>
#include "Constants.h"
class TransposeImGui : public Transpose {
public:
	void ActiveInterferes() {
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
	}
};
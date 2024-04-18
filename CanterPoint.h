#pragma once
#include "Point.h"
#include <map>
#include <string>
#include "FigureContainer.h"

class CenterPoint: public Point, public FigureContainer {
public:
	CenterPoint(Shader* shader, const char* name) : Point(shader, "##CenterPoint", "CenterPoint"){
		color = glm::vec4(1, 0, 0, 1);
	}

	CenterPoint(Shader* shader) : Point(shader, "##CenterPoint", "CenterPoint"){
		color = glm::vec4(1, 0, 0, 1);
	}

	void virtual Draw(const Camera& camera) {
		if (ContainerSize() > 0) {
			Update();
			Point::Draw(camera);
		}
	}

	void virtual ActiveImGui() {
		if (ContainerSize() < 1)
			return;
		ImGui::PushID("centerPoint");
		ImGui::BeginGroup(); {
			ImGui::Text("Control selected");
			// translation
			transpose->ActiveInterferes();
			if (transpose_last.GetPosition() != transpose->GetPosition()) {
				std::map<std::string, Figure* >::iterator iter;
				for (iter = selectedFigures.begin(); iter != selectedFigures.end(); iter++)
				{
					(*iter).second->transpose->
						MoveObjectPosition(transpose->GetPosition() - transpose_last.GetPosition());
				}
				transpose_last = *transpose;
			}


			if (ImGui::Checkbox("Local change", &localReference))
				ResetValues();
			RotationInterfers();

			ImGui::Text("Scale");

			if (ImGui::DragFloat("scale x##centerPoint", &scaleVec.x, 0.1f, M_ESP, M_FLOAT_MAX, "%.3f", ImGuiSliderFlags_AlwaysClamp) ||
				ImGui::DragFloat("scale y##centerPoint", &scaleVec.y, 0.1f, M_ESP, M_FLOAT_MAX, "%.3f", ImGuiSliderFlags_AlwaysClamp) ||
				ImGui::DragFloat("scale z##centerPoint", &scaleVec.z, 0.1f, M_ESP, M_FLOAT_MAX, "%.3f", ImGuiSliderFlags_AlwaysClamp)) {
				std::map<std::string, Figure* >::iterator iter;
				for (iter = selectedFigures.begin(); iter != selectedFigures.end(); iter++)
				{
					(*iter).second->ScaleAlong(ReferencePoint(*(*iter).second), scaleVec / LastScaleVec);
				}
				LastScaleVec = scaleVec;

				if (LastScaleVec.x == 0) { LastScaleVec.x = 1; }
				if (LastScaleVec.y == 0) { LastScaleVec.y = 1; }
				if (LastScaleVec.z == 0) { LastScaleVec.z = 1; }
			}
		}
		ImGui::EndGroup();
		ImGui::PopID();
	}

private:
	QuaternionRotationImGui localQuaternion;
	QuaternionRotationImGui localQuaternion_last;

	QuaternionRotationImGui globalQuaternion;
	QuaternionRotationImGui globalQuaternion_last;
	bool localReference = false;

	Transpose transpose_last;

	glm::vec3 scaleVec{ 1.0f };
	glm::vec3 LastScaleVec{ 1.0f };

	glm::vec3 ReferencePoint(const Figure& figure) {
		if (localReference)
			return figure.transpose->GetPosition();
		else
			return transpose->GetPosition();
	}

	void Update() override
	{
		if (!(valueAdded || valueErased))
			return;
		glm::vec3 position(0);
		std::map<std::string, Figure* >::iterator iter;
		for (iter = selectedFigures.begin(); iter != selectedFigures.end(); iter++)
		{
			position += (*iter).second->transpose->GetPosition();
		}
		float number = selectedFigures.size();
		transpose->SetObjectPosition(position / number);

		ResetValues();
		somethingHasChange = false;
		valueAdded = false;
		valueErased = false;
	}

	void RotationInterfers() {
		if (localReference) {
			if (localQuaternion.ActiveInterferes()) {
				std::map<std::string, Figure* >::iterator iter;
				for (iter = selectedFigures.begin(); iter != selectedFigures.end(); iter++)
				{
					(*iter).second->RotationAlong( localQuaternion.Get() * localQuaternion_last.Invers(), ReferencePoint(*(*iter).second));
				}
			}
			if (localQuaternion_last != localQuaternion)
				localQuaternion_last = localQuaternion;
		}
		else {
			if (globalQuaternion.ActiveInterferes()) {
				std::map<std::string, Figure* >::iterator iter;
				for (iter = selectedFigures.begin(); iter != selectedFigures.end(); iter++)
				{
					(*iter).second->RotationAlong( globalQuaternion.Get() * globalQuaternion_last.Invers(), ReferencePoint(*(*iter).second));
				}
			}
			if (globalQuaternion_last != globalQuaternion)
				globalQuaternion_last = globalQuaternion;
		}
	}

	void ResetValues() {
		localQuaternion.Reset();
		localQuaternion_last.Reset();

		globalQuaternion.Reset();
		globalQuaternion_last.Reset();

		LastScaleVec = scaleVec = glm::vec3(1, 1, 1);
		transpose_last = *transpose;
	}
};
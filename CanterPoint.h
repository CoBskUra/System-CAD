#pragma once
#include "Point.h"
#include <map>
#include <string>

class CenterPoint: public Point {
public:
	CenterPoint(Shader* shader, const char* name) : Point(shader, "##CenterPoint", "CenterPoint"){
		color = glm::vec4(1, 0, 0, 1);
	}

	CenterPoint(Shader* shader) : Point(shader, "##CenterPoint", "CenterPoint"){
		color = glm::vec4(1, 0, 0, 1);
	}

	bool Add(Figure* figure) {
		auto pair = selectedFigures.insert({ figure->GetUniqueName() , figure });
		figure->Mark();
		if (pair.second)
			UpdatePosition();
		return pair.second;
	}

	bool Erase(Figure* figure) {
		bool erased = selectedFigures.erase(figure->GetUniqueName());
		figure->UnMark();
		if (erased)
			UpdatePosition();
		return erased;
	}

	bool Contain(Figure* figure) {
		return selectedFigures.find(figure->GetUniqueName()) != selectedFigures.end();
	}

	void virtual Draw() {
		if (selectedFigures.size() > 0)
			Point::Draw();
	}


	void virtual ActiveImGui() {
		if (selectedFigures.size() < 1)
			return;

		ImGui::BeginGroup(); {
			ImGui::Text("Control selected");
			ImGui::Checkbox("Local change", &localReference);
			RotationInterfers();
			/*ImGui::Text("Rotation");
			if (ImGui::DragFloat("Rotate x", &angel.x, 0.1f, -M_PI, M_PI, "%.3f", ImGuiSliderFlags_AlwaysClamp)) {
				std::map<std::string, Figure* >::iterator iter;
				for (iter = selectedFigures.begin(); iter != selectedFigures.end(); iter++)
				{
					(*iter).second->RotationAlong(glm::vec3(1, 0, 0), ReferencePoint(*(*iter).second), angel.x - lastAngel.x);
				}
				lastAngel.x = angel.x;
			}

			if (ImGui::DragFloat("Rotate y", &angel.y, 0.1f, -M_PI, M_PI, "%.3f", ImGuiSliderFlags_AlwaysClamp)) {
				std::map<std::string, Figure* >::iterator iter;
				for (iter = selectedFigures.begin(); iter != selectedFigures.end(); iter++)
				{
					(*iter).second->RotationAlong(glm::vec3(0, 1, 0), ReferencePoint(*(*iter).second), angel.y - lastAngel.y);
				}
				lastAngel.y = angel.y;
			}

			if (ImGui::DragFloat("Rotate z", &angel.z, 0.1f, -M_PI, M_PI, "%.3f", ImGuiSliderFlags_AlwaysClamp)) {
				std::map<std::string, Figure* >::iterator iter;
				for (iter = selectedFigures.begin(); iter != selectedFigures.end(); iter++)
				{
					(*iter).second->RotationAlong(glm::vec3(0, 0, 1), ReferencePoint(*(*iter).second), angel.z - lastAngel.z);
				}
				lastAngel.z = angel.z;
			}*/

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
	}

private:
	std::map<std::string , Figure* > selectedFigures;
	QuaternionRotationImGui localQuaternion;
	QuaternionRotationImGui localQuaternion_last;

	QuaternionRotationImGui globalQuaternion;
	QuaternionRotationImGui globalQuaternion_last;
	bool localReference = false;

	glm::vec3 angel{ 0.0f };
	glm::vec3 lastAngel{ 0.0f };

	glm::vec3 scaleVec{ 1.0f };
	glm::vec3 LastScaleVec{ 1.0f };

	glm::vec3 ReferencePoint(const Figure& figure) {
		if (localReference)
			return figure.transpose.GetPosition();
		else
			return transpose.GetPosition();
	}

	void UpdatePosition()
	{
		glm::vec3 position(0);
		std::map<std::string, Figure* >::iterator iter;
		for (iter = selectedFigures.begin(); iter != selectedFigures.end(); iter++)
		{
			position += (*iter).second->transpose.GetPosition();
		}
		float number = selectedFigures.size();
		transpose.SetObjectPosition(position / number);
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
};


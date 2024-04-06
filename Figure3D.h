#pragma once
#include "Figure.h"
#include "ScaleImGui.h"
#include "RotationImGui.h"
#include "Camera.h"


class Figure3D :public Figure {
public:
	ScaleImGui scale;
	RotationImGui rotation;
	glm::mat4x4 globalRotation{ 1.0f };

	Figure3D(Shader* shader);

	void virtual RotationAlong(glm::vec3 axis, glm::vec3 rotationCenter, float angle);
	void virtual ScaleAlong(glm::vec3 scaleCenter, glm::vec3 scaleVec);
	void virtual MoveAlong(const Camera& camera, glm::vec3 direction);

	void virtual ActiveImGui();
	glm::mat4x4 virtual GetModelMatrix();
	glm::mat4x4 virtual GetModelMatrixInvers();

protected:

	Figure3D(Shader* shader, const char* uniqueName, const char* type);
};
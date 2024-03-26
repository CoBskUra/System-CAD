#pragma once
#include "Figure.h"
#include "ScaleImGui.h"
#include "RotationImGui.h"
#include "Camera.h"


class Figure3D :public Figure {
public:
	Figure3D(Shader* shader);

	void virtual RotationAlong(const Figure& center, const Camera& camera, float angle);
	void virtual ScaleAlong(const Figure& center, const Camera& camera, glm::vec3 scaleVec);
	void virtual MoveAlong(const Figure& center, const Camera& camera, glm::vec3 direction);

	void virtual ActiveImGui();
	glm::mat4x4 virtual GetModelMatrix();
	glm::mat4x4 virtual GetModelMatrixInvers();

protected:
	ScaleImGui scale;
	RotationImGui rotation;

	Figure3D(Shader* shader, const char* uniqueName, const char* type);
};
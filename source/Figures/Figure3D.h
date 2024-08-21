#ifndef Figure3D_CLASS_H
#define Figure3D_CLASS_H
#include "Figures/Figure.h"
#include "Transformations/ScaleImGui.h"
#include "Transformations/RotationImGui.h"
#include "Camera.h"
#include "Transformations/QuaternionRotationImGui.h"


class Figure3D :public Figure {
public:
	ScaleImGui scale;
	RotationImGui rotation;
	QuaternionRotationImGui quaternion;

	Figure3D();

	void virtual RotationAlong(glm::vec3 axis, glm::vec3 rotationCenter, float angle);
	void virtual RotationAlong(Quaternion q, glm::vec3 rotationCenter);

	void virtual ScaleAlong(glm::vec3 scaleCenter, glm::vec3 scaleVec);
	void virtual MoveAlong(const Camera& camera, glm::vec3 direction);

	void virtual ActiveImGui();
	glm::mat4x4 virtual GetModelMatrix();
	glm::mat4x4 virtual GetModelMatrixInvers();

protected:

	Figure3D(const char* uniqueName, FigureType type);
};

#endif
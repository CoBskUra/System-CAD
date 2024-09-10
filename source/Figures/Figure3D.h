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

	void RotationAlong(glm::vec3 axis, glm::vec3 rotationCenter, float angle) override;
	void RotationAlong(Quaternion q, glm::vec3 rotationCenter) override;

	void ScaleAlong(glm::vec3 scaleCenter, glm::vec3 scaleVec) override;
	void MoveAlong(const Camera& camera, glm::vec3 direction) override;

	void  ActiveImGui() override;
	glm::mat4x4  GetModelMatrix()  override;
	glm::mat4x4  GetModelMatrixInvers() override;

protected:

	Figure3D( FigureType type);
};

#endif
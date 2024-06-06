#pragma once

#include <ImGui/imgui_impl_glfw.h>
#include "Camera.h"
#include "Figure.h"

class StereoscopicView : public Camera
{
	enum class EyeSetings
	{
		monoEye,
		leftEye,
		righteEye
	};
	float rightPlane = 0.3f;
	float leftPlane = 0.1f;


	float topPlane = 0.2f;
	float bottomPlane = 0.1f;

	float convergence = 10;

	EyeSetings currentSettings;
	float eyesDistance = 1.0f;
	glm::vec3 monoEye;

	glm::mat4 ProjectionMatrix() const override;
	glm::mat4 ProjectionMatrixInvers() const override;
	void updateMatrixes() override;

	void SetParams(EyeSetings typeOfEye);
public:
	bool turnOn = false;


	StereoscopicView(const Camera& camera);
	Camera GetStereoCam(Camera& camera, bool leftEye);
	void ActiveInterferes() override;
	void operator=(const Camera& camera);
	void LeftEyeSeting();
	void RighteEyeSeting();
	void MonoEyeSetting();
	glm::vec3 SetMonoEye(glm::vec3 newPos);
};


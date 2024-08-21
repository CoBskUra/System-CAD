#include "StereoscoicView.h"

glm::mat4 StereoscopicView::ProjectionMatrix() const
{
	if (currentSettings == EyeSetings::monoEye)
		return Camera::ProjectionMatrix();

	glm::mat4 proj{ 1.0f };

	proj[0][0] = 2 * nearPlane / (rightPlane - leftPlane);
	proj[1][1] = 2 * nearPlane / (topPlane - bottomPlane);
	proj[3][3] = 0;

	proj[2][0] = (rightPlane + leftPlane) / (rightPlane - leftPlane);
	proj[2][1] = (topPlane + bottomPlane) / (topPlane - bottomPlane);
	proj[2][2] = (farPlane + nearPlane) / (farPlane - nearPlane);
	proj[2][3] = 1;

	proj[3][2] = (-2 * farPlane * nearPlane) / (farPlane - nearPlane);

	return proj;
}

glm::mat4 StereoscopicView::ProjectionMatrixInvers() const
{
	return glm::inverse(ProjectionMatrix());
}

void StereoscopicView::updateMatrixes()
{
	if (currentSettings == EyeSetings::monoEye)
		SetMonoEye(GetPosition());

	SetParams(currentSettings);
	Camera::updateMatrixes();
}

void StereoscopicView::SetParams(EyeSetings typeOfEye)
{
	topPlane = tanf(FOVRad * 0.5f) * nearPlane;
	bottomPlane = -topPlane;

	float a = aspect * tanf(FOVRad * 0.5f) * convergence;;

	float b = a - eyesDistance * 0.5f;
	float c = a + eyesDistance * 0.5f;


	switch (typeOfEye)
	{
	case StereoscopicView::EyeSetings::monoEye:
		rightPlane = tanf(FOVRad * 0.5f) / aspect;
		leftPlane = -rightPlane;
		break;
	case StereoscopicView::EyeSetings::leftEye:

		leftPlane = -c * nearPlane / convergence;
		rightPlane = b * nearPlane / convergence;
		break;
	case StereoscopicView::EyeSetings::righteEye:

		leftPlane = -b * nearPlane / convergence;
		rightPlane = c * nearPlane / convergence;
		break;
	default:
		break;
	}

	switch (typeOfEye)
	{
	default:
		break;
	}
}

StereoscopicView::StereoscopicView(const Camera& camera): Camera(camera)
{
	SetParams(currentSettings);
	updateMatrixes();
	monoEye = camera.GetPosition();
}

Camera StereoscopicView::GetStereoCam(Camera& camera, bool leftEye)
{
	Camera tmp = camera;
	if (leftEye) {
		glm::vec3 oldPos = camera.GetPosition();
		tmp.SetPosition(oldPos - glm::vec3(eyesDistance * 0.5f, 0, 0));
		tmp.updateMatrixes();
	}
	else {
		glm::vec3 oldPos = camera.GetPosition();
		tmp.SetPosition(oldPos + glm::vec3(eyesDistance * 0.5f, 0, 0));
		tmp.updateMatrixes();
	}
	return tmp;
}

void StereoscopicView::ActiveInterferes()
{
	if (!turnOn)
		return;
	ImGui::PushID("StereoscopicView");
	ImGui::BeginGroup();
	{
		/*OrientationImgui();
		PositionImgui();
		ScaleImgui();
		MoveSettingsImgui();*/

	/*	ImGui::Text("Projection");
		ImGui::DragFloat("Near Plane", &nearPlane, 0.1f, M_ESP, farPlane);
		ImGui::DragFloat("Far Plane", &farPlane, 0.1f, nearPlane, M_FLOAT_MAX);

		ImGui::DragFloat("Top Plane", &topPlane, 0.1f, bottomPlane, M_FLOAT_MAX);
		ImGui::DragFloat("Botton Plane", &bottomPlane, 0.1f, M_ESP, topPlane);

		ImGui::DragFloat("Rigth Plane", &rightPlane, 0.1f, leftPlane, M_FLOAT_MAX);
		ImGui::DragFloat("Left Plane", &leftPlane, 0.1f, M_ESP, rightPlane);*/

		Camera::ActiveInterferes();

		ImGui::Text("Steroscopy");
		if (ImGui::DragFloat("Eyes distance", &eyesDistance, 0.01f, M_ESP, M_FLOAT_MAX, "%.3f", ImGuiSliderFlags_AlwaysClamp))
			updateMatrixes();

		if(ImGui::DragFloat("Covered", &convergence, 0.001f, 0.001, M_FLOAT_MAX, "%.5f", ImGuiSliderFlags_AlwaysClamp))
			updateMatrixes();

	}
	ImGui::EndGroup();
	ImGui::PopID();
}

void StereoscopicView::operator=(const Camera& camera)
{
	hasBeenUpdated = false;
	Position = camera.GetPosition();
	SetOrientation( camera.GetOrientation());
	Up = camera.GetUp();

	SetFov(camera.GetFov());

	SetNearPlane(camera.GetNearPlane());
	SetFarPlane(camera.GetFarPlane());

	SetAspect(camera.GetAspect());

	SetScale(camera.GetScale());

	SetParams(currentSettings);
	updateMatrixes();

	monoEye = camera.GetPosition();
}

void StereoscopicView::LeftEyeSeting()
{
	SetPosition(monoEye - glm::vec3(eyesDistance * 0.5f, 0, 0));
	currentSettings = EyeSetings::leftEye;
	updateMatrixes();
}

void StereoscopicView::RighteEyeSeting()
{
	SetPosition(monoEye + glm::vec3(eyesDistance * 0.5f, 0, 0));
	currentSettings = EyeSetings::righteEye;
	updateMatrixes();
}

void StereoscopicView::MonoEyeSetting()
{
	SetPosition(monoEye);
	currentSettings = EyeSetings::monoEye;
	updateMatrixes();
}

glm::vec3 StereoscopicView::SetMonoEye(glm::vec3 newPos)
{
	auto tmp = monoEye;
	monoEye = newPos;
	return tmp;
}

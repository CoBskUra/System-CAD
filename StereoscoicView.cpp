#include "StereoscoicView.h"

glm::mat4 StereoscopicView::ProjectionMatrix()
{
	glm::mat4 proj{ 1.0f };

	proj[0][0] = 2* nearPlane / (rightPlane - leftPlane);
	proj[1][1] = 2 * nearPlane / (topPlane - bottomPlane);
	proj[3][3] = 0;

	//proj[2][0] = (rightPlane + leftPlane) / (rightPlane - leftPlane);
	//proj[2][1] = (topPlane + bottomPlane) / (topPlane - bottomPlane);
	proj[2][2] = (farPlane + nearPlane) / (farPlane - nearPlane);
	proj[2][3] = 1;

	proj[3][2] = (-2 * farPlane * nearPlane) / (farPlane - nearPlane);

	return proj;
}

void StereoscopicView::SetParams(Camera& camera)
{
	/*nearPlane = camera.GetNearPlane();
	farPlane = camera.GetFarPlane();

	topPlane = tan(camera.FOVRad) * nearPlane;
	bottomPlane = -topPlane;

	rightPlane = topPlane * camera.aspect;
	leftPlane = -rightPlane;*/
}

void StereoscopicView::Draw(GLFWwindow* window, Camera& camera, Figure* figure)
{
	Camera tmp = camera;
	if (turnOn)
	{
		//save camera setings
		glm::vec3 oldPos = camera.GetPosition();
		//glm::mat4 oldProj = camera.projection;

		//// new projection
		//SetParams(camera);
		//camera.projection = proj;
		float blend = 0.8;
		// left eye
		glm::vec4 color = figure->GetShowColor();
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		figure->SetShowColor(glm::vec4(0.8f, 0, 0, 1) * blend + color * (1 - blend));
		tmp.SetPosition(oldPos - glm::vec3(eyesDistance * 0.5f, 0, 0));
		tmp.updateMatrixes();
		figure->Draw(window, tmp);

		// right eye
		figure->SetShowColor(glm::vec4(0, 0, 0.8f, 1) * blend + color * (1 - blend));
		tmp.SetPosition(oldPos + glm::vec3(eyesDistance * 0.5f, 0, 0));
		tmp.updateMatrixes();
		figure->Draw(window, tmp);
		glDisable(GL_BLEND);
		// revers changes
		//camera.Position = oldPos;
		////camera.projection = oldProj;
		//camera.updateMatrixes();
	}
	else
		figure->Draw(window, camera);
}

bool StereoscopicView::Interferes()
{
	if (!turnOn)
		return false;
	ImGui::PushID("StereoscopicView");
	ImGui::Begin("Stereoscopy", &turnOn);
	{
		/*ImGui::DragFloat("Far Plane", &farPlane, 0.1f, nearPlane, M_FLOAT_MAX);
		ImGui::DragFloat("Near Plane", &nearPlane, 0.1f, -M_FLOAT_MAX, farPlane);
		ImGui::Separator();

		ImGui::DragFloat("Top Plane", &topPlane, 0.1f, bottomPlane, M_FLOAT_MAX);
		ImGui::DragFloat("Bottom Plane", &bottomPlane, 0.1f, -M_FLOAT_MAX, topPlane);
		ImGui::Separator();

		ImGui::DragFloat("Right Plane", &rightPlane, 0.1f, leftPlane, M_FLOAT_MAX);
		ImGui::DragFloat("Left Plane", &leftPlane, 0.1f, -M_FLOAT_MAX, rightPlane);
		ImGui::Separator();*/

		ImGui::DragFloat("Eyes distance", &eyesDistance, 0.1f, -M_FLOAT_MAX, M_FLOAT_MAX);
	}
	ImGui::End();
	ImGui::PopID();
	return false;
}

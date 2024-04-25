#include "VirtualPoint.h"

void VirtualPoint::SetObjectPosition(float x, float y, float z)
{
	glm::vec3 lastPos = transpose->GetPosition();
	Point::SetObjectPosition(x, y, z);
	glm::vec3 currentPos = transpose->GetPosition();
	glm::vec3 delta = currentPos - lastPos;
	for (auto iter = connectedPoints.begin(); iter != connectedPoints.end(); iter++) {
		(*iter)->transpose->MoveObjectPosition(delta);
	}
}

bool VirtualPoint::Inputs(GLFWwindow* window, const Camera& camera)
{
	ImGuiIO& io = ImGui::GetIO();

	if (io.WantCaptureMouse)
		return false;

	if (firstClick && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		if (OpenGLHelper::QuadraticDistanceFromMouse(window, camera, *transpose) > 0.001f)
			return false;
		firstClick = false;
	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		auto castToScreen = camera.GetCameraMatrix() * glm::vec4(transpose->GetPosition(), 1);
		castToScreen /= castToScreen.w;

		glm::vec4 mousePos{
			OpenGLHelper::MousePositionOnScreen(window),
			castToScreen.z,
			1.0f
		};


		mousePos = camera.GetCameraMatrixInvers() * mousePos;
		mousePos /= mousePos.w;
		transpose->SetObjectPosition(mousePos);
		return true;
	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
	{
		firstClick = true;
	}
	return false;
	return false;
}

VirtualPoint::VirtualPoint(Shader* shader) : Point(shader, "##VirtualPoint", FigureType::Point)
{
	SetName("VirtualPoint");
}

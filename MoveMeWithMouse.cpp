#include "MoveMeWithMouse.h"

bool MoveMeWithMouse::Move(GLFWwindow* window, const Camera& camera, Figure* me)
{
	ImGuiIO& io = ImGui::GetIO();

	if (io.WantCaptureMouse)
		return false;

	if (firstClick && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		if (OpenGLHelper::QuadraticDistanceFromMouse(window, camera, *me->transpose) > minDist)
			return false;
		firstClick = false;
	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		auto castToScreen = camera.GetCameraMatrix() * glm::vec4(me->transpose->GetPosition(), 1);
		castToScreen /= castToScreen.w;

		glm::vec4 mousePos{
			OpenGLHelper::MousePositionOnScreen(window),
			castToScreen.z,
			1.0f
		};


		mousePos = camera.GetCameraMatrixInvers() * mousePos;
		mousePos /= mousePos.w;
		me->transpose->SetObjectPosition(mousePos);
		return true;
	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
	{
		firstClick = true;
	}
	return false;
}

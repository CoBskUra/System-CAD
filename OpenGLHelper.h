#pragma once
#include <GLFW/glfw3.h>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_int2.hpp>
#include <glm/ext/vector_double2.hpp>
static class OpenGLHelper {
public:
	static glm::vec2  MousePositionOnScreen(GLFWwindow* window) {
		glm::dvec2 mousPos;
		glfwGetCursorPos(window, &mousPos.x, &mousPos.y);
		glm::ivec2 windowSize;
		glfwGetWindowSize(window, &windowSize.x, &windowSize.y);

		auto castMousePos = glm::vec2(mousPos.x - windowSize.x * 0.5f, -mousPos.y + windowSize.y * 0.5f);
		return glm::vec2(castMousePos.x / (windowSize.x * 0.5f), castMousePos.y / (windowSize.y * 0.5f));
	}
};
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

	static float QuadraticDistanceFromMouse(GLFWwindow* window, const Camera& camera, const Transpose& transpose) {
		glm::vec2 mousePosOnScreen = MousePositionOnScreen( window);
		glm::vec4 objectPosOnScreen = camera.GetCameraMatrix() * transpose.Get() * glm::vec4(0, 0, 0, 1);
		objectPosOnScreen /= objectPosOnScreen.w;

		float length = powf(objectPosOnScreen.x - mousePosOnScreen.x, 2) + powf(objectPosOnScreen.y - mousePosOnScreen.y, 2);
		return length;
	}
	
	static void AddVecToVector(std::vector<float>& vector, const glm::vec3& pos) {
		vector.push_back(pos.x); vector.push_back(pos.y); vector.push_back(pos.z);
	}

	static glm::vec3 TakeLastVecFromVector(std::vector<float>& vector) {
		glm::vec3 pos{ vector.at(vector.size() - 3), vector.at(vector.size() - 2), vector.at(vector.size() - 1) };
		vector.pop_back();
		vector.pop_back();
		vector.pop_back();

		return pos;
	}
};
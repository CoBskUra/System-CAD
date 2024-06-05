#pragma once

#include <ImGui/imgui_impl_glfw.h>
#include "Camera.h"
#include "Figure.h"

class StereoscopicView
{
	float nearPlane = 0.1f;
	float farPlane = 100.0f;

	float rightPlane = 0.3f;
	float leftPlane = 0.1f;


	float topPlane = 0.2f;
	float bottomPlane = 0.1f;

	float eyesDistance = 1.0f;
	glm::mat4 ProjectionMatrix();

	void SetParams(Camera& camera);
public:
	bool turnOn = false;

	void Draw(GLFWwindow* window, Camera& camera, Figure* figure);
	bool Interferes();
};


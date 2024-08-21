#pragma once
#include <imgui_impl_glfw.h>
#include "Camera.h"
#include "Figures/Figure.h"
#include "Helper/OpenGLHelper.h"

class MoveMeWithMouse {
	bool firstClick = true;
	float minDist = 0.001f;
public:
	bool Move(GLFWwindow* window, const Camera& camera, Figure* me);
};
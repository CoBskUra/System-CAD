#pragma once
#include "BezierBase.h"
#include "Container/FigureContainer.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <vector>
#include "ShaderRefrence/StaticShaders.h"

class BezierC0 : public BezierBase {
	Shader* lineDrawing = StaticShaders::GetPointerToBezierCurve();
	int numberOfVertexes = 0;
protected:
	glm::vec4 curveColor{ 1, 0, 1, 1 };

	Shader* shader_bezier2D = StaticShaders::GetPointerToBezier2D();
	VAO vao_bezier3D;

	Shader* shader_bezier3D = StaticShaders::GetPointerToBezier3D();
	VAO vao_bezier2D;
	bool bezier2D_on = false;
public:
	BezierC0(const char* name);
	BezierC0();

	void virtual Draw(GLFWwindow* window, const Camera& camera);
protected:
	BezierC0(const char* name, const char* uniqueName, FigureType type);
	void virtual CreateBezier();
};
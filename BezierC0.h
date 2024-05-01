#pragma once
#include "BezierBase.h"
#include "FigureContainer.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <vector>

class BezierC0 : public BezierBase {
	Shader lineDrawing{"bezierCurve_vert.glsl", "bezierCurve_frag.glsl"};
	int numberOfVertexes = 0;
protected:
	glm::vec4 curveColor{ 1, 0, 1, 1 };
public:
	BezierC0(Shader* shader, const char* name);
	BezierC0(Shader* shader);

	void virtual Draw(GLFWwindow* window, const Camera& camera);
protected:
	BezierC0(Shader* shader, const char* name, const char* uniqueName, FigureType type);
	void virtual CreateBezier();
};
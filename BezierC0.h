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
	bool bezier2D_on = false;
	Shader shader_bezier2D = Shader({
		{"bezier3d_vert.glsl", "VERTEX", GL_VERTEX_SHADER} ,
		{"bezier3d_frag.glsl", "FRAGMENT", GL_FRAGMENT_SHADER} ,
		{"bezier2d_tc.glsl", "TESELATION_CONTROL", GL_TESS_CONTROL_SHADER},
		{"bezier2d_te.glsl", "TESELATION_CONTROL", GL_TESS_EVALUATION_SHADER}
		});
	VAO vao_bezier2D;
public:
	BezierC0(Shader* shader, const char* name);
	BezierC0(Shader* shader);

	void virtual Draw(GLFWwindow* window, const Camera& camera);
protected:
	BezierC0(Shader* shader, const char* name, const char* uniqueName, FigureType type);
	void virtual CreateBezier();
};
#pragma once
#include "Figure.h"
#include "FigureContainer.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


#include <iostream>
#include <vector>

class BezierCurve : public Figure, public  FigureContainer {
	Shader lineDrawing{"bezierCurve_vert.glsl", "bezierCurve_frag.glsl"};
	glm::vec4 curveColor{ 1, 0, 1, 1 };
	bool showBezierCurve = true;
	bool showBezierPol = true;
	int numberOfVertexes = 0;
public:
	BezierCurve(Shader* shader, const char* name);
	BezierCurve(Shader* shader);

	void virtual Draw(const Camera& camera);
	void virtual ActiveImGui();
	bool virtual Inputs(GLFWwindow* window, const Camera& camera);

	bool IsValid(Figure* figure) override;
	void Update() override;

protected:
	BezierCurve(Shader* shader, const char* uniqueName, const char* type);

private:

	void CreateBezierCurve();
};
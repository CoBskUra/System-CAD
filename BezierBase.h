#pragma once
#include "CanterPoint.h"
#include "FigureContainer.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <vector>

class BezierBase : public CenterPoint {
public:
	bool showBezierCurve = true;
	bool showBezierPol = true;

	BezierBase( const char* name);
	BezierBase();

	void virtual Draw(GLFWwindow* window, const Camera& camera);
	void virtual ActiveImGui();
	bool virtual Inputs(GLFWwindow* window, const Camera& camera);

	bool IsValid(Figure* figure) override;
	void virtual Update();
	bool virtual AddContainer(FigureContainer* fc);
	void MarkFigure(Figure* f) override;

	void virtual ChangeShowBezierCurve();
	void virtual ChangeShowBezierPol();
protected:
	BezierBase(const char* name, const char* uniqueName, FigureType type);
	void virtual CreateBezier();
	void SelectedPoints();
};
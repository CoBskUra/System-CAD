#pragma once
#include "Point.h"
#include "FigureFunctions/MoveMeWithMouse.h"

class VirtualPoint: public Point {
	MoveMeWithMouse figureController;
public:
	std::vector<Figure*> connectedPoints;
	void virtual SetObjectPosition(float x, float y, float z);
	bool virtual  Inputs(GLFWwindow* window, const Camera& camera);
	VirtualPoint();
};
#pragma once
#include "Point.h"
#include "OpenGLHelper.h"

class VirtualPoint: public Point {
	bool firstClick = true;
public:
	std::vector<Figure*> connectedPoints;
	void virtual SetObjectPosition(float x, float y, float z);
	bool virtual  Inputs(GLFWwindow* window, const Camera& camera);
	VirtualPoint(Shader* shader);
};
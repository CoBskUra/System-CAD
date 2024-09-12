#pragma once
#include "BezierBase.h"

class BezierCurve : public BezierBase {
public:
	virtual glm::vec3 Derivative(float t) = 0;
	virtual glm::vec3 PositionOnCurve(float t) = 0;
	BezierCurve(const char* name, FigureType type);
};
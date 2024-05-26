#pragma once
#include "BezierBase.h"
#include "BezierC2.h"
#include "ShaderCreator.h"

class BezierInterpolated: public BezierBase
{
	std::vector<VirtualPoint*> virtualPoints;
	int virtualPointsNumber = 0;
	BezierC0 innerBezierC0;
public:
	BezierInterpolated( const char* name);
	BezierInterpolated();

	void virtual Draw(GLFWwindow* window, const Camera& camera);

	void virtual CreateBezier();
	void virtual ChangeShowBezierCurve();

protected:
	BezierInterpolated(const char* name, const char* uniqueName, FigureType type);
	void virtual ChangeShowBezierPol();
	float TakePointDelta(int i);
	float TakePointDelta(std::vector<glm::vec3> points, int i);
	void AddPoint(BezierC0& bezier, glm::vec3 pos);
};


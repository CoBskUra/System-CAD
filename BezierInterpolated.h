#pragma once
#include "BezierBase.h"
#include "BezierC2.h"
#include "ShaderCreator.h"

class BezierInterpolated: public BezierBase
{
	std::vector<VirtualPoint*> virtualPoints;
	int virtualPointsNumber = 0;
	BezierC2 innerBezierC2;
public:
	BezierInterpolated( const char* name);
	BezierInterpolated();

	void virtual Draw(GLFWwindow* window, const Camera& camera);

	void virtual CreateBezier();
	void virtual ChangeShowBezierC0();

protected:
	BezierInterpolated(const char* name, const char* uniqueName, FigureType type);
	void virtual ChangeShowBezierPol();
};


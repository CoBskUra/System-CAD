#pragma once
#include "BezierBase.h"
#include "BezierC0.h"
#include "CanterPoint.h"
#include "ShaderCreator.h"
#include "VirtualPoint.h"
#include "StaticShaders.h"

class BezierC2: public BezierBase
{
	BezierC0 innerBezierC0;
	std::vector<VirtualPoint*> virtualPoints;
	int virtualPointsNumber = 0;
	glm::vec4 virtualPointColor = { 1,0,1,1 };


public:
	bool showVirtualPointsInterfers = false;
	bool showVirtualPoints = true;

	BezierC2(const char* name);
	BezierC2();
	virtual ~BezierC2();

	void virtual Draw(GLFWwindow* window, const Camera& camera);
	void virtual ActiveImGui();
	bool virtual Inputs(GLFWwindow* window, const Camera& camera);

	void virtual CreateBezier();
	void virtual ChangeShowBezierCurve();
	void virtual ChangeShowBezierPol();

protected:
	BezierC2( const char* name, const char* uniqueName, FigureType type);
};


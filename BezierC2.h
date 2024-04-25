#pragma once
#include "BezierC0.h"
#include "CanterPoint.h"
#include "ShaderCreator.h"
#include "VirtualPoint.h"

class BezierC2: public BezierC0
{
	Shader point_shader;
	Shader bezierC0_shader;
	BezierC0 innerBezierC0;
	std::vector<VirtualPoint*> virtualPoints;
	int virtualPointsNumber = 0;

	bool showVirtualPointsInterfers = false;
	bool showVirtualPoints = true;

	void SelectableList();
public:
	BezierC2(Shader* shader, const char* name);
	BezierC2(Shader* shader);
	virtual ~BezierC2();

	void virtual Draw(GLFWwindow* window, const Camera& camera);
	void virtual ActiveImGui();
	bool virtual Inputs(GLFWwindow* window, const Camera& camera);

	void virtual CreateBezier();
	void virtual ChangeShowBezierC0();

protected:
	BezierC2(Shader* shader, const char* name, const char* uniqueName, FigureType type);
	void virtual ChangeShowBezierPol();
};


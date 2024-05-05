#pragma once
#include "BezierBase.h"
#include "BezierC0.h"
#include "CanterPoint.h"
#include "ShaderCreator.h"
#include "VirtualPoint.h"
#include "StaticShaders.h"

class BezierC2: public BezierBase
{
	Shader* point_shader = StaticShaders::GetPointerToPoint();
	Shader* bezierC0_shader = StaticShaders::GetPointerToBezier3D();
	BezierC0 innerBezierC0;
	std::vector<VirtualPoint*> virtualPoints;
	int virtualPointsNumber = 0;
	glm::vec4 virtualPointColor = { 1,0,1,1 };

	bool showVirtualPointsInterfers = false;
	bool showVirtualPoints = true;

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


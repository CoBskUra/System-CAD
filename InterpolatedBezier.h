#pragma once
#include "BezierBase.h"
#include "BezierC2.h"
#include "ShaderCreator.h"

class InterpolatedBezier: public BezierBase
{
	Shader bezierC0_shader;
	BezierC2 innerBezierC0;
public:
	InterpolatedBezier( const char* name);
	InterpolatedBezier();

	void virtual Draw(GLFWwindow* window, const Camera& camera);

	void virtual CreateBezier();
	void virtual ChangeShowBezierC0();

protected:
	InterpolatedBezier(const char* name, const char* uniqueName, FigureType type);
	void virtual ChangeShowBezierPol();
};


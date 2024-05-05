#include "InterpolatedBezier.h"

InterpolatedBezier::InterpolatedBezier(Shader* shader, const char* name) :InterpolatedBezier(shader, name, "##InterpolatedBezier", FigureType::InterpolatedBezier)
{
}

InterpolatedBezier::InterpolatedBezier(Shader* shader):InterpolatedBezier(shader, "InterpolatedBezier")
{
}

void InterpolatedBezier::Draw(GLFWwindow* window, const Camera& camera)
{
}

void InterpolatedBezier::CreateBezier()
{

}

void InterpolatedBezier::ChangeShowBezierC0()
{
	BezierBase::ChangeShowBezierC0();
}

InterpolatedBezier::InterpolatedBezier(Shader* shader, const char* name, const char* uniqueName, FigureType type) :
	BezierBase(shader, name, uniqueName, type),
	bezierC0_shader{ ShaderCreator::Bezier_C0() },
	innerBezierC0{ &bezierC0_shader }
{
}

void InterpolatedBezier::ChangeShowBezierPol()
{
	BezierBase::ChangeShowBezierPol();
}

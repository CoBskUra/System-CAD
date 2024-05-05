#include "InterpolatedBezier.h"

InterpolatedBezier::InterpolatedBezier(const char* name) :InterpolatedBezier( name, "##InterpolatedBezier", FigureType::InterpolatedBezier)
{
}

InterpolatedBezier::InterpolatedBezier():InterpolatedBezier( "InterpolatedBezier")
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

InterpolatedBezier::InterpolatedBezier( const char* name, const char* uniqueName, FigureType type) :
	BezierBase(name, uniqueName, type),
	bezierC0_shader{ ShaderCreator::Bezier_C0() }
{
}

void InterpolatedBezier::ChangeShowBezierPol()
{
	BezierBase::ChangeShowBezierPol();
}

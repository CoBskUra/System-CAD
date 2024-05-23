#include "StaticShaders.h"
#include <GLFW/glfw3.h>

bool StaticShaders::haseBeenInit = false;

Shader StaticShaders::bezier2D;

Shader StaticShaders::bezier3D;

Shader StaticShaders::bezierCurve;

Shader StaticShaders::cursor;

Shader StaticShaders::defaultShader;

Shader StaticShaders::point;

Shader StaticShaders::infinityGrid;

Shader StaticShaders::Torus;

Shader StaticShaders::BezierSurfaceC0;

Shader* StaticShaders::GetPointerToBezier2D() {
	return &bezier2D;
}
Shader* StaticShaders::GetPointerToBezier3D(){
	return &bezier3D;
}

Shader* StaticShaders::GetPointerToBezierCurve() {
	return &bezierCurve;
}

Shader* StaticShaders::GetPointerToCursor() {
	return &cursor;
}

Shader* StaticShaders::GetPointerToDefaultShader() {
	return &defaultShader;
}

Shader* StaticShaders::GetPointerToPoint() {
	return &point;
}

Shader* StaticShaders::GetPointerToInfinityGrid() {
	return &infinityGrid;
}
Shader* StaticShaders::GetPointerToTorus() {
	return &Torus;
}

Shader* StaticShaders::GetPointerToBezierSurfaceC0()
{
	return &BezierSurfaceC0;
}

void StaticShaders::Init()
{
	if (haseBeenInit)
		return;
	haseBeenInit = true;

	bezier2D = {
	{
		{"bezier3d_vert.glsl", "VERTEX", GL_VERTEX_SHADER} ,
		{"bezier3d_frag.glsl", "FRAGMENT", GL_FRAGMENT_SHADER} ,
		{"bezier2d_tc.glsl", "TESELATION_CONTROL", GL_TESS_CONTROL_SHADER},
		{"bezier2d_te.glsl", "TESELATION_EVALUATION", GL_TESS_EVALUATION_SHADER}
	}
	};

	bezier3D = {
	{
		{"bezier3d_vert.glsl", "VERTEX", GL_VERTEX_SHADER} ,
		{"bezier3d_frag.glsl", "FRAGMENT", GL_FRAGMENT_SHADER} ,
		{"bezier3d_tc.glsl", "TESELATION_CONTROL", GL_TESS_CONTROL_SHADER},
		{"bezier3d_te.glsl", "TESELATION_EVALUATION", GL_TESS_EVALUATION_SHADER}
	}
	};

	bezierCurve = { "bezierCurve_vert.glsl", "bezierCurve_frag.glsl" };

	cursor = { "cursor_vert.glsl", "cursor_frag.glsl" };

	defaultShader = { "default.vert", "default.frag" };

	point = { "point_vert.glsl", "point_frag.glsl" };

	infinityGrid = { "infinityGrid_vert.glsl", "infinityGrid_frag.glsl" };

	Torus = {
	{
		{"simple_vert.glsl", "VERTEX", GL_VERTEX_SHADER} ,
		{"simple_frag.glsl", "FRAGMENT", GL_FRAGMENT_SHADER}
	}
	};

	BezierSurfaceC0 = {
	{
		{"bezier3d_vert.glsl", "VERTEX", GL_VERTEX_SHADER} ,
		{"bezier3d_frag.glsl", "FRAGMENT", GL_FRAGMENT_SHADER} ,
		{"bezierSurfaceC0_tc.glsl", "TESELATION_CONTROL", GL_TESS_CONTROL_SHADER},
		{"bezierSurfaceC0_te.glsl", "TESELATION_EVALUATION", GL_TESS_EVALUATION_SHADER}
	}
	};
}

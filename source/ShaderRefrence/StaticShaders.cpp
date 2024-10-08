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
Shader StaticShaders::BezierSurfaceC2;
Shader StaticShaders::GregoryPatch;

const Shader& StaticShaders::GetBezier2D() {
	return bezier2D;
}
const Shader& StaticShaders::GetBezier3D(){
	return bezier3D;
}

const Shader& StaticShaders::GetBezierCurve() {
	return bezierCurve;
}

const Shader& StaticShaders::GetCursor() {
	return cursor;
}

const Shader& StaticShaders::GetDefaultShader() {
	return defaultShader;
}

const Shader& StaticShaders::GetPoint() {
	return point;
}

const Shader& StaticShaders::GetInfinityGrid() {
	return infinityGrid;
}
const Shader& StaticShaders::GetTorus() {
	return Torus;
}

const Shader& StaticShaders::GetBezierSurfaceC0()
{
	return BezierSurfaceC0;
}

const Shader& StaticShaders::GetBezierSurfaceC2()
{
	return BezierSurfaceC2;
}

const Shader& StaticShaders::GetGregoryPatch()
{
	return GregoryPatch;
}

void StaticShaders::Init()
{
	if (haseBeenInit)
		return;
	haseBeenInit = true;

	bezier2D = {
	{
		{"../resource/Shaders/bezier3d_vert.glsl", "VERTEX", GL_VERTEX_SHADER} ,
		{"../resource/Shaders/bezier3d_frag.glsl", "FRAGMENT", GL_FRAGMENT_SHADER} ,
		{"../resource/Shaders/bezier2d_tc.glsl", "TESELATION_CONTROL", GL_TESS_CONTROL_SHADER},
		{"../resource/Shaders/bezier2d_te.glsl", "TESELATION_EVALUATION", GL_TESS_EVALUATION_SHADER}
	}
	};

	bezier3D = {
	{
		{"../resource/Shaders/bezier3d_vert.glsl", "VERTEX", GL_VERTEX_SHADER} ,
		{"../resource/Shaders/bezier3d_frag.glsl", "FRAGMENT", GL_FRAGMENT_SHADER} ,
		{"../resource/Shaders/bezier3d_tc.glsl", "TESELATION_CONTROL", GL_TESS_CONTROL_SHADER},
		{"../resource/Shaders/bezier3d_te.glsl", "TESELATION_EVALUATION", GL_TESS_EVALUATION_SHADER}
	}
	};

	bezierCurve = 
	{ 
		"../resource/Shaders/bezierCurve_vert.glsl", 
		"../resource/Shaders/bezierCurve_frag.glsl" 
	};

	cursor = 
	{ 
		"../resource/Shaders/cursor_vert.glsl",
		"../resource/Shaders/cursor_frag.glsl" 
	};

	defaultShader = 
	{ 
		"../resource/Shaders/default.vert", 
		"../resource/Shaders/default.frag"
	};

	point = 
	{ 
		"../resource/Shaders/point_vert.glsl", 
		"../resource/Shaders/point_frag.glsl" 
	};

	infinityGrid = 
	{ 
		"../resource/Shaders/infinityGrid_vert.glsl",
		"../resource/Shaders/infinityGrid_frag.glsl" 
	};

	Torus = {
	{
		{"../resource/Shaders/torus_vert.glsl", "VERTEX", GL_VERTEX_SHADER} ,
		{"../resource/Shaders/torus_frag.glsl", "FRAGMENT", GL_FRAGMENT_SHADER}
	}
	};

	BezierSurfaceC0 = {
	{
		{"../resource/Shaders/bezierSurfaceC0_vert.glsl", "VERTEX", GL_VERTEX_SHADER} ,
		{"../resource/Shaders/bezierSurfaceC0_frag.glsl", "FRAGMENT", GL_FRAGMENT_SHADER} ,
		{"../resource/Shaders/bezierSurfaceC0_tc.glsl", "TESELATION_CONTROL", GL_TESS_CONTROL_SHADER},
		{"../resource/Shaders/bezierSurfaceC0_te.glsl", "TESELATION_EVALUATION", GL_TESS_EVALUATION_SHADER}
	}
	};

	BezierSurfaceC2 = {
	{
		{"../resource/Shaders/bezierSurfaceC2_vert.glsl", "VERTEX", GL_VERTEX_SHADER} ,
		{"../resource/Shaders/bezierSurfaceC2_frag.glsl", "FRAGMENT", GL_FRAGMENT_SHADER} ,
		{"../resource/Shaders/bezierSurfaceC2_tc.glsl", "TESELATION_CONTROL", GL_TESS_CONTROL_SHADER},
		{"../resource/Shaders/bezierSurfaceC2_te.glsl", "TESELATION_EVALUATION", GL_TESS_EVALUATION_SHADER}
	}
	};

	GregoryPatch = {
	{
		{"../resource/Shaders/GregoryPatch_vert.glsl", "VERTEX", GL_VERTEX_SHADER} ,
		{"../resource/Shaders/GregoryPatch_frag.glsl", "FRAGMENT", GL_FRAGMENT_SHADER} ,
		{"../resource/Shaders/GregoryPatch_tc.glsl", "TESELATION_CONTROL", GL_TESS_CONTROL_SHADER},
		{"../resource/Shaders/GregoryPatch_te.glsl", "TESELATION_EVALUATION", GL_TESS_EVALUATION_SHADER}
	}
	};
}

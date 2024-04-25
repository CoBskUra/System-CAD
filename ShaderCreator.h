#pragma once
#include "Shader.h"

static class ShaderCreator {
public:
	static Shader CenterPoint() {
		return Shader("point_vert.glsl", "point_frag.glsl");
	}

	static Shader Point() {
		return Shader("point_vert.glsl", "point_frag.glsl");
	}

	static Shader Bezier_C0() {
		return Shader({
		{"bezier3d_vert.glsl", "VERTEX", GL_VERTEX_SHADER} ,
		{"bezier3d_frag.glsl", "FRAGMENT", GL_FRAGMENT_SHADER} ,
		{"bezier3d_tc.glsl", "TESELATION_CONTROL", GL_TESS_CONTROL_SHADER},
		{"bezier3d_te.glsl", "TESELATION_CONTROL", GL_TESS_EVALUATION_SHADER}
			});
	}
};
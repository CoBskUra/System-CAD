#pragma once
#include "ShaderManadement/Shader.h"

static struct StaticShaders{
	static void Init(); // u¿yj przed u¿yciem shaderów
	static const Shader& GetBezier2D();
	static const Shader& GetBezier3D();
	static const Shader& GetBezierCurve();
	static const Shader& GetCursor();
	static const Shader& GetDefaultShader();
	static const Shader& GetPoint();
	static const Shader& GetInfinityGrid();
	static const Shader& GetTorus();
	static const Shader& GetBezierSurfaceC0();
	static const Shader& GetBezierSurfaceC2();
	static const Shader& GetGregoryPatch();
private:
	static bool haseBeenInit;
	static Shader bezier2D;
	static Shader bezier3D;
	static Shader bezierCurve;
		   
	static Shader cursor;
		   
	static Shader defaultShader;
		   
	static Shader point;
		   
	static Shader infinityGrid;
	static Shader Torus;

	static Shader BezierSurfaceC0;
	static Shader BezierSurfaceC2;
	static Shader GregoryPatch;
};
#pragma once
#include "ShaderManadement/Shader.h"

static struct StaticShaders{
	static void Init(); // u¿yj przed u¿yciem shaderów
	static Shader* GetPointerToBezier2D();
	static Shader* 	GetPointerToBezier3D();
	static Shader* 	GetPointerToBezierCurve();

	static Shader* 	GetPointerToCursor();

	static Shader* 	GetPointerToDefaultShader();

	static Shader* 	GetPointerToPoint();

	static Shader* 	GetPointerToInfinityGrid();
	static Shader* 	GetPointerToTorus();

	static Shader* GetPointerToBezierSurfaceC0();
	static Shader* GetPointerToBezierSurfaceC2();
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

	static const char* path;
};
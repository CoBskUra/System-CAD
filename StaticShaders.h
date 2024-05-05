#pragma once
#include "Shader.h"

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
};
#pragma once
#include "Figure.h"
#include "StaticShaders.h"

class InfinityGrid:public Figure
{
public:
	InfinityGrid(const char* name);
	InfinityGrid();

	void  Draw(GLFWwindow* window, const Camera& camera) override;

	bool Inputs(GLFWwindow* window, const Camera& camera) override;
protected:
	InfinityGrid(const char* uniqueName, FigureType type);

private:
	VAO vaoInfinityGrid;
	Shader* infinityGridShader = StaticShaders::GetPointerToInfinityGrid();

	void CreateInfinityGrid();
};


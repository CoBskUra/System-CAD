#pragma once
#include "Figures/Figure.h"
#include "ShaderRefrence/StaticShaders.h"

class InfinityGrid:public Figure
{
public:
	InfinityGrid(const char* name);
	InfinityGrid();

	void  Draw(GLFWwindow* window, const Camera& camera) override;

	bool Inputs(GLFWwindow* window, const Camera& camera) override;
protected:
	InfinityGrid(FigureType type);

private:
	VAO vao;
	const Shader& shader = StaticShaders::GetInfinityGrid();

	void CreateInfinityGridVAO();
};


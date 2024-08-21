#include "InfinityGrid.h"

InfinityGrid::InfinityGrid(const char* name) : InfinityGrid("##InfinityGrid", FigureType::UnchangeObject)
{
	CreateInfinityGrid();
	SetName(name);
}

InfinityGrid::InfinityGrid() : InfinityGrid("InfinityGrid")
{
}

void InfinityGrid::Draw(GLFWwindow* window, const Camera& camera)
{
	infinityGridShader->Activate();
	vaoInfinityGrid.Bind();
	{
		glUniformMatrix4fv(glGetUniformLocation(infinityGridShader->ID, "camMatrixInvers"),
			1, GL_FALSE, glm::value_ptr(camera.GetCameraMatrixInvers()));
		camera.SaveMatrixToShader(infinityGridShader->ID);
		glUniform1f(glGetUniformLocation(infinityGridShader->ID, "near"), camera.GetNearPlane());
		glUniform1f(glGetUniformLocation(infinityGridShader->ID, "far"), camera.GetFarPlane());
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
	vaoInfinityGrid.Unbind();
}

bool InfinityGrid::Inputs(GLFWwindow* window, const Camera& camera) {
	return false;
}

InfinityGrid::InfinityGrid(const char* uniqueName, FigureType type) : Figure(uniqueName, type) {
}

void InfinityGrid::CreateInfinityGrid()
{
	std::vector<float> infinityGrid{
		1, 1, 0,
		-1, -1, 0,
		-1, 1, 0,

		-1, -1, 0,
		1, 1, 0,
		1, -1, 0
	};

	vaoInfinityGrid.Bind();
	VBO vboInfinityGrid(infinityGrid, GL_STATIC_DRAW);

	vaoInfinityGrid.LinkAttrib(0, 3, GL_FLOAT, false, 3 * sizeof(float), 0);

	vaoInfinityGrid.Unbind(); vboInfinityGrid.Unbind();
}
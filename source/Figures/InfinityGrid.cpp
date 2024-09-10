#include "InfinityGrid.h"

InfinityGrid::InfinityGrid(const char* name) : InfinityGrid( FigureType::UnchangeObject)
{
	CreateInfinityGridVAO();
	SetName(name);
}

InfinityGrid::InfinityGrid() : InfinityGrid("InfinityGrid")
{
}

void InfinityGrid::Draw(GLFWwindow* window, const Camera& camera)
{
	shader.Activate();
	vao.Bind();
	{
		glUniformMatrix4fv(glGetUniformLocation(shader.ID, "camMatrixInvers"),
			1, GL_FALSE, glm::value_ptr(camera.GetCameraMatrixInvers()));
		camera.SaveMatrixToShader(shader.ID);
		glUniform1f(glGetUniformLocation(shader.ID, "near"), camera.GetNearPlane());
		glUniform1f(glGetUniformLocation(shader.ID, "far"), camera.GetFarPlane());
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
	vao.Unbind();
}

bool InfinityGrid::Inputs(GLFWwindow* window, const Camera& camera) {
	return false;
}

InfinityGrid::InfinityGrid( FigureType type) : Figure( type) {
}

void InfinityGrid::CreateInfinityGridVAO()
{
	std::vector<float> infinityGrid{
		1, 1, 0,
		-1, -1, 0,
		-1, 1, 0,

		-1, -1, 0,
		1, 1, 0,
		1, -1, 0
	};

	vao.Bind();
	VBO vboInfinityGrid(infinityGrid, GL_STATIC_DRAW);

	vao.LinkAttrib(0, 3, GL_FLOAT, false, 3 * sizeof(float), 0);

	vao.Unbind(); vboInfinityGrid.Unbind();
}
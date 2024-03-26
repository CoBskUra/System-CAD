#pragma once
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
class Scale
{
public:
	Scale();

	glm::mat4x4 Get();
	glm::mat4x4 GetInvers();

	void SetScale(glm::vec3 vec);
	void SetScale(float x, float y, float z);

protected:
	glm::vec3 ScaleVec;
	glm::mat4 Matrix;
	glm::mat4 Matrix_invers;
};


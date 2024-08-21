#pragma once
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>

class Transpose
{public:
	Transpose();

	glm::mat4 Get() const;
	glm::mat4 GetInvers() const;
	glm::vec3 GetPosition() const;

	void SetObjectPosition(glm::vec3 vec);
	void virtual SetObjectPosition(float x, float y, float z);
	void MoveObjectPosition(glm::vec3 vec);
	void MoveObjectPosition(float x, float y, float z);
protected:
	glm::vec3 ObjectPosition;
	glm::mat4 Transform;
};


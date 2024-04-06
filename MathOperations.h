#pragma once
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
static class MathOperations {
public:
	static glm::vec3 RotationAlongAxis(glm::vec3 v, float rad, glm::vec3 axis)
	{
		axis = glm::normalize(axis);
		float s = sinf(rad);
		float c = cosf(rad);

		return (1 - c) * glm::dot(v, axis) * axis + c * v + s * glm::cross(axis, v);
	}

	static glm::mat4x4 RotationMatrixAlongAxis(float rad, glm::vec3 axis)
	{
		float s = sinf(rad);
		float c = cosf(rad);
		float t = 1 - c;
		
		glm::mat4x4 R{ 1.0f };
		R[0][0] = t*axis.x*axis.x + c;
		R[1][1] = t * axis.y * axis.y + c;
		R[2][2] = t * axis.z * axis.z + c;
		R[3][3] = 1.0f;

		R[1][0] = t * axis.x * axis.y - s * axis.z;
		R[2][0] = t * axis.x * axis.z - s * axis.y;
		R[2][1] = t * axis.y * axis.z - s * axis.x;

		R[0][1] = -R[1][0];
		R[0][2] = -R[2][0];
		R[1][2] = -R[2][1];

		return R;
	}
};
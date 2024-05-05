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

	static void InversThreeDiagonalMatrix(const int n, glm::vec3 x[],
		const float a[], const float b[],
		const float c[], glm::vec3 scratch[]) {
		/*
		 solves Ax = d, where A is a tridiagonal matrix consisting of vectors a, b, c
		 X = number of equations
		 x[] = initially contains the input v, and returns x. indexed from [0, ..., X - 1]
		 a[] = subdiagonal, indexed from [1, ..., X - 1]
		 b[] = main diagonal, indexed from [0, ..., X - 1]
		 c[] = superdiagonal, indexed from [0, ..., X - 2]
		 scratch[] = scratch space of length X, provided by caller, allowing a, b, c to be const
		 not performed in this example: manual expensive common subexpression elimination
		 */
		scratch[0] = glm::vec3{ 1, 1, 1 } * (c[0] / b[0]);
		x[0] = x[0] / b[0];

		/* loop from 1 to X - 1 inclusive */
		for (int ix = 1; ix < n; ix++) {
			if (ix < n - 1) {
				scratch[ix] = c[ix] / (b[ix] - a[ix] * scratch[ix - 1]);
			}
			x[ix] = (x[ix] - a[ix] * x[ix - 1]) / (b[ix] - a[ix] * scratch[ix - 1]);
		}

		/* loop from X - 2 to 0 inclusive */
		for (int ix = n - 2; ix >= 0; ix--)
			x[ix] -= scratch[ix] * x[ix + 1];
	}
};
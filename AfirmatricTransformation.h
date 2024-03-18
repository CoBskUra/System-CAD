#pragma once
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include <cfenv>
#include <cmath>
#include <iostream>
#include "Constants.h"

class AfirmatricTransformation
{
public:

	AfirmatricTransformation();

	void SetRotation_X(float angle);
	void SetRotation_Y(float angle);
	void SetRotation_Z(float angle);

	void Rotate_X(float angle);
	void Rotate_Y(float angle);
	void Rotate_Z(float angle);

	void SetObjectPosition(glm::vec3 vec);

	void SetObjectPosition(float x, float y, float z);

	void MoveObjectPosition(glm::vec3 vec);

	void MoveObjectPosition(float x, float y, float z);

	void SetScale(glm::vec3 vec);

	void SetScale(float x, float y, float z);

	const glm::mat4 GetModelMatrix() const { return M; }
	const glm::mat4 GetModelMatrixInvers() const { return M_invers; }




protected:
	//const float M_PI = 3.1415926535897932384626433832795;
	const float esp = 1 >> 3;

	glm::mat4 Rx;
	glm::mat4 Rx_invers;;
	float alfa_x;

	glm::mat4 Ry;
	glm::mat4 Ry_invers;
	float alfa_y;

	glm::mat4 Rz;
	glm::mat4 Rz_invers;
	float alfa_z;

	glm::vec3 ObjectPosition;
	glm::mat4 Transform;
	glm::mat4 Transform_invers;;


	glm::vec3 ScaleVec;
	glm::mat4 Scale;
	glm::mat4 Scale_invers;


	glm::mat4 M;
	glm::mat4 M_invers;

private:
	float SimplifiedAngle(float angle);
	void CalculateM();
};


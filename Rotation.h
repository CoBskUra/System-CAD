#pragma once
#include <glm/ext/matrix_float4x4.hpp>
#include "Constants.h"
class Rotation
{
public:
	Rotation();
	void SetRotation_X(float angle);
	void SetRotation_Y(float angle);
	void SetRotation_Z(float angle);

	void Rotate_X(float angle);
	void Rotate_Y(float angle);
	void Rotate_Z(float angle);

	glm::mat4x4 Get_Rx();
	glm::mat4x4 Get_Ry();
	glm::mat4x4 Get_Rz();
	glm::mat4x4 Get_R();


	glm::mat4x4 Get_RxInvers();
	glm::mat4x4 Get_RyInvers();
	glm::mat4x4 Get_RzInvers();
	glm::mat4x4 Get_RInvers();


protected:
	glm::mat4 Rx;
	float alfa_x;

	glm::mat4 Ry;
	float alfa_y;

	glm::mat4 Rz;
	float alfa_z;
private:
	float SimplifiedAngle(float angle);

};


#ifndef Quaternion_CLASS_H
#define Quaternion_CLASS_H
#include <glm/ext/matrix_float4x4.hpp>
#include "Constants.h"
class Quaternion
{
public:
	Quaternion();
	Quaternion(float r, float i = 0, float j = 0, float k = 0);
	Quaternion(float angle, glm::vec3 axis);
	Quaternion(glm::vec3 values);
	Quaternion(glm::vec4 values);

	Quaternion(glm::mat3 RotationMatrix);
	Quaternion(glm::mat4 RotationMatrix);

	glm::mat4 virtual GetEulerRotation() const;
	Quaternion virtual Invers() const;
	glm::vec3 RotatePoint(glm::vec3 point) const;
	Quaternion RotatedAlong(float angle, glm::vec3 axis) const;

	void SetRotationAlong(float angle, glm::vec3 axis);
	glm::vec4 virtual GetAngleRepresentation();
	void Normalize();

	float r() const { return values[0]; }
	float i() const{ return values[1]; }
	float j() const{ return values[2]; }
	float k() const{ return values[3]; }

	void r(float a) { values[0] = a; }
	void i(float a) { values[1] = a; }
	void j(float a) { values[2] = a; }
	void k(float a) { values[3] = a; }


	friend Quaternion operator*(Quaternion const& q1, Quaternion const& q2);

	friend Quaternion operator+(Quaternion const& q1, Quaternion const& q2);


	friend bool operator==(Quaternion const& q1, Quaternion const& q2);
	friend bool operator!=(Quaternion const& q1, Quaternion const& q2);

protected:
	glm::vec4 values{ 0.0f };
};




#endif

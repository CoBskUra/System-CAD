#pragma once
#include "glm/glm.hpp"

class IntersectionAble {
public:
	virtual glm::vec3 Parametrization	(float v, float u) = 0;
	virtual glm::vec3 Derivative_u(float v, float u) = 0;
	virtual glm::vec3 Derivative_uu(float v, float u) = 0;

	virtual glm::vec3 Derivative_v(float v, float u) = 0;
	virtual glm::vec3 Derivative_vv(float v, float u) = 0;

	virtual glm::vec3 Derivative_vu(float v, float u) = 0;
	virtual glm::vec3 Derivative_uv(float v, float u) = 0;
	virtual glm::vec2 Field_u() = 0;
	virtual glm::vec2 Field_v() = 0;
	virtual glm::bvec2 CanWrap() = 0;
};
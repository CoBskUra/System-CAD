#pragma once
#include "IntersectionAble.h"

class IntersectionConstFun : public IntersectionAble {
public:
	glm::vec3 point;
	float const min = -1;
	float const max = 1;

	IntersectionConstFun(glm::vec3 point) {
		this->point = point;
	}
	glm::vec3 Parametrization(float v, float u) override {
		return point;
	}
	glm::vec3 Derivative_u(float v, float u) override {
		return {0, 0, 0};
	}
	glm::vec3 Derivative_uu(float v, float u) override {
		return { 0, 0, 0 };
	}

	glm::vec3 Derivative_v(float v, float u) override {
		return { 0, 0, 0 };
	}
	glm::vec3 Derivative_vv(float v, float u) override {
		return { 0, 0, 0 };
	}

	glm::vec3 Derivative_vu(float v, float u) override {
		return { 0, 0, 0 };
	}
	glm::vec3 Derivative_uv(float v, float u) override {
		return { 0, 0, 0 };
	}
	glm::vec2 Field_u() override {
		return { min, max };
	}
	glm::vec2 Field_v() override {
		return { min, max };
	}
	glm::bvec2 CanWrap() override {
		return {true, true};
	}
};
#pragma once
#include "IntersectionAble.h"

class IntersectionAblePuffed:public IntersectionAble 
{
	IntersectionAble* m_intersectionAble;
	float m_r;
public:
	IntersectionAblePuffed(IntersectionAble* intersectionAble, float r);

	glm::vec3 Normal(float v, float u);
	glm::vec3 Normal_v(float v, float u);
	glm::vec3 Normal_u(float v, float u);


	glm::vec3 Parametrization(float v, float u) override;
	 glm::vec3 Derivative_u(float v, float u) override;
	 glm::vec3 Derivative_uu(float v, float u) override;
	 
	 glm::vec3 Derivative_v(float v, float u) override;
	 glm::vec3 Derivative_vv(float v, float u) override;
	 
	 glm::vec3 Derivative_vu(float v, float u) override;
	 glm::vec3 Derivative_uv(float v, float u) override;

	 glm::vec2 Field_u() override;
	 glm::vec2 Field_v() override;
	 glm::bvec2 CanWrap() override;
};
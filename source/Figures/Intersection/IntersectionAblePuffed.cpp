#include "IntersectionAblePuffed.h"

IntersectionAblePuffed::IntersectionAblePuffed(IntersectionAble* intersectionAble, float r)
{
	this->m_intersectionAble = intersectionAble;
	this->m_r = r;
}

glm::vec3 IntersectionAblePuffed::Normal(float v, float u)
{
	auto  pv = this->m_intersectionAble->Derivative_v(v, u);
	auto  pu = this->m_intersectionAble->Derivative_u(v, u);
	auto cos = glm::cross(pv, pu);
	return  cos;
}

glm::vec3 IntersectionAblePuffed::Normal_v(float v, float u)
{
	return glm::cross(this->m_intersectionAble->Derivative_vv(v, u), this->m_intersectionAble->Derivative_u(v, u)) + glm::cross(this->m_intersectionAble->Derivative_v(v, u), this->m_intersectionAble->Derivative_uv(v, u));
}

glm::vec3 IntersectionAblePuffed::Normal_u(float v, float u)
{
	return glm::cross(this->m_intersectionAble->Derivative_vu(v, u), this->m_intersectionAble->Derivative_u(v, u)) + glm::cross(this->m_intersectionAble->Derivative_v(v, u), this->m_intersectionAble->Derivative_uu(v, u));
}

glm::vec3 IntersectionAblePuffed::Parametrization(float v, float u)
{
	return this->m_intersectionAble->Parametrization(v, u) + glm::normalize(Normal(v,u)) * this->m_r;
}

glm::vec3 IntersectionAblePuffed::Derivative_u(float v, float u)
{
	/*auto normal = Normal(v, u);
	auto normal_u = Normal_u(v, u);
	auto nn = glm::dot(normal, normal);*/
	//return this->m_intersectionAble->Derivative_u(v, u) + this->m_r*(normal_u * nn - normal * glm::dot(normal, normal_u)) / (nn * sqrtf(nn));
	float h = 0.000001;
	return this->m_intersectionAble->Derivative_u(v, u) + (glm::normalize(Normal(v, u + h)) - glm::normalize(Normal(v, u))) * this->m_r / h;
	//return this->m_intersectionAble->Derivative_u(v, u);
}

glm::vec3 IntersectionAblePuffed::Derivative_uu(float v, float u)
{
	throw "no implementation";
}

glm::vec3 IntersectionAblePuffed::Derivative_v(float v, float u)
{
	/*auto normal = Normal(v, u);
	auto normal_v = Normal_v(v, u);
	auto nn = glm::dot(normal, normal);*/
	//return this->m_intersectionAble->Derivative_v(v, u);// +
		//this->m_r*(normal_v * nn - normal * glm::dot(normal, normal_v)) / (nn * sqrtf(nn));
	float h = 0.000001;
	return this->m_intersectionAble->Derivative_v(v, u) + (glm::normalize(Normal(v + h, u)) - glm::normalize(Normal(v, u))) * this->m_r / h;
	//return this->m_intersectionAble->Derivative_v(v, u);
}

glm::vec3 IntersectionAblePuffed::Derivative_vv(float v, float u)
{
	throw "no implementation";
}

glm::vec3 IntersectionAblePuffed::Derivative_vu(float v, float u)
{
	throw "no implementation";
}

glm::vec3 IntersectionAblePuffed::Derivative_uv(float v, float u)
{
	throw "no implementation";
}

glm::vec2 IntersectionAblePuffed::Field_u()
{
	return this->m_intersectionAble->Field_u();
}

glm::vec2 IntersectionAblePuffed::Field_v()
{
	return this->m_intersectionAble->Field_v();
}

glm::bvec2 IntersectionAblePuffed::CanWrap()
{
	return this->m_intersectionAble->CanWrap();
}

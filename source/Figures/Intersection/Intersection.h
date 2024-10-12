#pragma once
#include "IntersectionAble.h"
#include "IntersectionConstFun.h"
#include <glm/ext/matrix_float4x4.hpp>
#include <iostream>
#include <random>
#include <Figures/Point.h>

// find 0.5*x^t*A*x + x^t*b
class Intersection {
	float epsilon = 0.00001;
	const float c = (sqrtf(5) - 1.0f) * 0.5f;
	const int maxIterations_IntersectionFrame = 1000;
	const int maxIterations_NextIntersectionParams = 100;
	const int maxIterations_FirstIntersectionPoint = 1000;
	const int retries_FirstIntersectionPoint = 5;
	const int retires2_FirstIntersectionPoint = 2;
	const int randomTriesToFindMatch = 100;
	const int samples = 50;
	Point p1, p2;

	glm::vec3 pos_a;
	glm::vec3 pos_b;

	glm::vec3 a_d_v;
	glm::vec3 a_d_u;
	glm::vec3 a_d_vv;
	glm::vec3 a_d_vu;
	glm::vec3 a_d_uv;
	glm::vec3 a_d_uu;

	glm::vec3 b_d_v;
	glm::vec3 b_d_u;
	glm::vec3 b_d_vv;
	glm::vec3 b_d_vu;
	glm::vec3 b_d_uv;
	glm::vec3 b_d_uu;

	inline void CalculateObjects_derivative(const glm::vec4& params, IntersectionAble* object_a, IntersectionAble* object_b);
	inline void CalculateObjects_derivativeSingle(const glm::vec4& params, IntersectionAble* object_a, IntersectionAble* object_b);

	inline float StepMinimalization(const glm::vec4& params, const glm::vec4& d, float min, float max, IntersectionAble* object_a, IntersectionAble* object_b);

	inline float  Derivative_x();
	inline float  Derivative_y();
	inline float  Derivative_z();
	inline float  Derivative_w();

	inline float  Derivative_xx();
	inline float  Derivative_xy();
	inline float  Derivative_xz();
	inline float  Derivative_xw();

	inline float  Derivative_yx();
	inline float  Derivative_yy();
	inline float  Derivative_yz();
	inline float  Derivative_yw();

	inline float  Derivative_zx();
	inline float  Derivative_zy();
	inline float  Derivative_zz();
	inline float  Derivative_zw();

	inline float  Derivative_wx();
	inline float  Derivative_wy();
	inline float  Derivative_wz();
	inline float  Derivative_ww();

	inline glm::vec4 RandomParamsCloseTo(glm::vec4 params, float length, IntersectionAble* object_a, IntersectionAble* object_b);
	inline glm::vec4 Clamp(glm::vec4 params, IntersectionAble* object_a, IntersectionAble* object_b);
	inline glm::vec4 Clamp(glm::vec4 params, IntersectionAble* object_a, IntersectionAble* object_b, bool& clamped);
	inline float Clamp(const float& a, const float& min, const float& max);
	inline float Wrap(const float& a, const float& min, const float& max);
	inline bool IsCreatedLoop(const std::vector<glm::vec2>& points, IntersectionAble* object);

public:
	glm::vec4 FirstIntersectionPoint(IntersectionAble* object_a, IntersectionAble* object_b, glm::vec4 params = glm::vec4{ 0,0,0,0 }, bool derivativeStop = false);

	std::pair<std::vector<glm::vec2>, std::vector<glm::vec2>>  IntersectionFrame(glm::vec4 firstIntersection, IntersectionAble* object_a, IntersectionAble* object_b, float step, float eps);

	inline glm::vec4 NextIntersectionParams(glm::vec4 intersectionParams, IntersectionAble* object_a, IntersectionAble* object_b, float step, float eps, bool flipDirection = false);

	bool IsIntersected(IntersectionAble* object_a, IntersectionAble* object_b);
	bool IsIntersected(IntersectionAble* object_a, IntersectionAble* object_b, glm::vec4 params);
	inline glm::mat4 Hessian(const glm::vec4& params, IntersectionAble* object_a, IntersectionAble* object_b);
	inline glm::vec4 Derivative(const glm::vec4& params, IntersectionAble* object_a, IntersectionAble* object_b);
	inline float Function(const glm::vec4& params, IntersectionAble* object_a, IntersectionAble* object_b);
	glm::vec4 ParamsCloseToPoint(glm::vec3 point, IntersectionAble* object_a, IntersectionAble* object_b);
	glm::vec4 RandomTheClosetToEachOther(IntersectionAble* object_a, IntersectionAble* object_b);
	glm::vec4 RandomTheClosetToPoint(glm::vec3 point, IntersectionAble* object_a, IntersectionAble* object_b);
	glm::vec4 TheFaresParams(IntersectionAble* object_a);
};
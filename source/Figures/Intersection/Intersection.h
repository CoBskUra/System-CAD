#pragma once
#include "IntersectionAble.h"
#include "IntersectionConstFun.h"
#include <glm/ext/matrix_float4x4.hpp>
#include <iostream>
#include <random>
#include <Figures/Point.h>
#include <Figures/Bezier/BezierCurve.h>

struct cureIntersectionInfo {
	glm::vec2 pos;
	BezierCurve* bezier_1;
	float t1_1;
	float t1_2;
	BezierCurve* bezier_2;
	float t2_1;
	float t2_2;

	std::pair<BezierCurve*, float> MoveToNextCurve(BezierCurve* curve, float t = -1) {
		if (curve == bezier_1 && curve == bezier_2) {
			if (fabsf(t1_1 - t) < fabsf(t2_1 - t)) {
				return { bezier_2 , t2_2 };
			}
			else
				return { bezier_1 , t1_2 };
		}

		if (curve == bezier_1)
			return { bezier_2 , t2_2 };
		else if(curve == bezier_2)
			return { bezier_1 , t1_2 };
		return { nullptr, -1 };
	}

	bool ShouldMove(BezierCurve* curve, float t, float dis = 0.49f) {
		if (curve == bezier_1 && curve == bezier_2)
			return fabsf(t1_1 - t) < dis || fabsf(t2_1 - t) < dis;

		if (curve == bezier_1)
			return fabsf(t1_1 - t) < dis;
		else if (curve == bezier_2)
			return fabsf(t2_1 - t) < dis;
		return false;
	}
};

struct vectorIntersectionInfo {
	std::vector<glm::vec2>* m_vector_1;
	std::pair<int, int> m_id_1;

	std::vector<glm::vec2>* m_vector_2;
	std::pair<int, int> m_id_2;

	vectorIntersectionInfo(std::vector<glm::vec2>& vector_1,
		int id1_1,
		int id1_2,
		std::vector<glm::vec2>& vector_2,
		int id2_1,
		int id2_2) : m_vector_1(&vector_1), m_vector_2(&vector_2)
	{
		m_id_1 = { id1_1, id1_2 };
		m_id_2 = {id2_1, id2_2};
	}


	vectorIntersectionInfo(std::vector<glm::vec2>& vector_1,
		std::pair<int, int> id_1,
		std::vector<glm::vec2>& vector_2,
		std::pair<int, int> id_2
	) : m_vector_1(&vector_1), m_vector_2(&vector_2)
	{
		m_id_1 = id_1;
		m_id_2 = id_2;
	}

	std::optional<std::pair< std::vector<glm::vec2>*, std::pair<int, int>>>
		MoveToNextCurve(std::vector<glm::vec2>* vector, glm::vec2 p) {

		if (vector == m_vector_1 &&
			(p == m_vector_1->at(m_id_1.first) || p == m_vector_1->at(m_id_1.second))) {
			return std::pair< std::vector<glm::vec2>*, std::pair<int, int>>( m_vector_2, m_id_2 ) ;
		}
		else if (vector == m_vector_2 &&
			(p == m_vector_2->at(m_id_2.first) || p == m_vector_2->at(m_id_2.second))) {
			return { { m_vector_1, m_id_1} };
		}
		
		return {};
	}

	bool ShouldMove(std::vector<glm::vec2>* vector, glm::vec2 p) {

		if (vector == m_vector_1 &&
			(p == m_vector_1->at(m_id_1.first) || p == m_vector_1->at(m_id_1.second)))
			return true;
		else if (vector == m_vector_2 &&
			(p == m_vector_2->at(m_id_2.first) || p == m_vector_2->at(m_id_2.second)))
			return true;
		return false;
	}
};

// find 0.5*x^t*A*x + x^t*b
class Intersection {
	float epsilon = 0.00001;
	const float c = (sqrtf(5) - 1.0f) * 0.5f;
	const int maxIterations_IntersectionFrame = 800;
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
	inline glm::vec2 Clamp(glm::vec2 params, IntersectionAble* object_a, bool& clamped);
	inline float Clamp(const float& a, const float& min, const float& max);
	inline float Wrap(const float& a, const float& min, const float& max);
	inline bool IsCreatedLoop(const std::vector<glm::vec2>& points, IntersectionAble* object);

public:
	bool IsLoop(glm::vec2 p0, glm::vec2 p1, glm::vec2 p2);
	glm::vec4 FirstIntersectionPoint(IntersectionAble* object_a, IntersectionAble* object_b, glm::vec4 params = glm::vec4{ 0,0,0,0 }, float functionEpsilon = -1, bool derivativeStop = false);

	std::pair<std::vector<glm::vec2>, std::vector<glm::vec2>>  IntersectionFrame(glm::vec4 firstIntersection, IntersectionAble* object_a, IntersectionAble* object_b, float step, float eps);

	inline glm::vec4 NextIntersectionParams(glm::vec4 intersectionParams, IntersectionAble* object_a, IntersectionAble* object_b, float step, float eps, bool flipDirection, bool &suces);

	bool IsIntersected(IntersectionAble* object_a, IntersectionAble* object_b);
	bool IsIntersected(IntersectionAble* object_a, IntersectionAble* object_b, glm::vec4 params);
	inline glm::mat4 Hessian(const glm::vec4& params, IntersectionAble* object_a, IntersectionAble* object_b);
	inline glm::vec4 Derivative(const glm::vec4& params, IntersectionAble* object_a, IntersectionAble* object_b);
	inline float Function(const glm::vec4& params, IntersectionAble* object_a, IntersectionAble* object_b);
	glm::vec4 ParamsCloseToPoint(glm::vec3 point, IntersectionAble* object_a, IntersectionAble* object_b);
	glm::vec2 ParamsCloseToPointSample(glm::vec3 point, IntersectionAble* object_a);
	glm::vec4 RandomTheClosetToEachOther(IntersectionAble* object_a, IntersectionAble* object_b);
	glm::vec4 RandomTheClosetToPoint(glm::vec3 point, IntersectionAble* object_a, IntersectionAble* object_b);
	glm::vec4 TheFaresParams(IntersectionAble* object_a);
	glm::vec3 RayIntersection(IntersectionAble* object_a, glm::vec3 pos, glm::vec3 direction, bool& found);
	glm::vec2 IntersectionBezier_2D_XZ(glm::vec2 params, BezierCurve* bezierCurve, glm::vec2 pos, glm::vec2 direction, float r);
	glm::vec2 IntersectionBezier_2D_XZ(glm::vec2 params, BezierCurve* bezierCurve_1, BezierCurve* bezierCurve_2, float r);
	float TheClosetTo(BezierCurve* bezierCurve, glm::vec3 pos);

	std::vector<glm::vec2> PosibleIntersections(BezierCurve* bezierCurve_1, BezierCurve* bezierCurve_2, float r = 0);
	std::vector<vectorIntersectionInfo> PosibleIntersections(std::vector<glm::vec2>& points_1, std::vector<glm::vec2>& points_2);
	std::vector<cureIntersectionInfo> PosibleIntersections_2(BezierCurve* bezierCurve_1, BezierCurve* bezierCurve_2, float r = 0);
	inline glm::vec3 MoveAcrossNormal(float t, BezierCurve* bezierCurve, float r) {
		auto tmp = glm::normalize(Normal_2D(bezierCurve, t));
		glm::vec3 n1{ tmp.x, 0, tmp.y };

		return bezierCurve->PositionOnCurve(t) + n1 * r;
	}

	inline glm::vec2 Normal_2D(BezierCurve* bezierCurve, float t)
	{
		glm::vec3 d = bezierCurve->Derivative(t);
		glm::vec2 n{ -d.z, d.x };
		return n;
	}

	inline glm::vec2 Normal_2D_Derivative(BezierCurve* bezierCurve, float t)
	{
		glm::vec3 d = bezierCurve->Derivative_2(t);
		glm::vec2 n{ -d.z, d.x };
		return n;
	}

	inline glm::vec2 NormalizeNormal_2D_Derivative(BezierCurve* bezierCurve, float t)
	{
		glm::vec2 n = Normal_2D(bezierCurve, t);
		glm::vec2 nd = Normal_2D_Derivative(bezierCurve, t);

		float n_dot = glm::dot(n, n);
		float tmp = (n.x * nd.y - n.y * nd.x) / (n_dot * sqrtf(n_dot));

		return glm::vec2{-n.y, n.x} * tmp;
	}

	inline glm::vec2 Two_bezierIntersectionDerivative(BezierCurve* bezierCurve, float t, float r)
	{
		glm::vec2 n = Normal_2D(bezierCurve, t);
		glm::vec2 nd = Normal_2D_Derivative(bezierCurve, t);
		glm::vec3 tmp = bezierCurve->Derivative(t);
		glm::vec2 bd{ tmp.x, tmp.z };
		float n_dot = glm::dot(n, n);
		float factor = (n_dot * sqrtf(n_dot));;

		return bd + r * glm::vec2{
			n.y * (n.y* nd.x - n.x * nd.y),
			n.x * (n.x * nd.y - n.y * nd.x)
		} / factor;
	}

	/*inline bool IsInsideABSquare(const glm::vec2& a, const glm::vec2& b, const glm::vec2& c) {
		glm::vec2 BottomLeft;
		BottomLeft.x = fminf(a.x, b.x);
		BottomLeft.y = fmin(a.y, b.y);

		glm::vec2 UpRight;
		UpRight.x = fmaxf(a.x, b.x);
		UpRight.y = fmaxf(a.y, b.y);

		if(BottomLeft.x <= c.x && UpRight.x >= c.x && BottomLeft.y <= c.y && UpRight.y >=  )

	}*/

	inline glm::vec2 VectorIntersection(const glm::vec2& a, const glm::vec2& b, const glm::vec2& c, const glm::vec2& d, bool& isOneIntersection, float& s) {

		isOneIntersection = false;
		glm::vec2 v0 = b - a;
		glm::vec2 v1 = d - c;
		glm::vec2 an{ -v0.y, v0.x };

		if (glm::dot(an, v1) == 0) // równoleg³e
		{
			return a + v0 * 0.5f;
		}
		glm::vec2 ac = c - a;
		glm::vec2 ad = d - a;
		float dis_0 = glm::dot(ac, an);
		float dis_1 = glm::dot(ad, an);
		if (dis_0 * dis_1 > 0) // nieprzecinaj¹ siê 
		{
			return { 0,0 };
		}

		dis_0 = fabsf(dis_0);
		dis_1 = fabsf(dis_1);
		float dis = dis_0 + dis_1;
		glm::vec2 point = (c * dis_1 + d * dis_0) / dis;
		s = -1;
		if (v0.x != 0) {
			s = (point.x - a.x) / v0.x;
		}
		else if (v0.y != 0) {
			s = (point.y - a.y) / v0.y;
		}
		else
		{
			return point;
		}

		if (s <= 1 && s >= 0)
			isOneIntersection = true;

		return point;
	}
};
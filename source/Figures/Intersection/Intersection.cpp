#include "Intersection.h"

glm::vec4 Intersection::FirstIntersectionPoint(IntersectionAble* object_a, IntersectionAble* object_b, glm::vec4 startParametrs, bool derivativeStop) {

	glm::vec4 params = startParametrs;
	for (int k = 0; k < retries_FirstIntersectionPoint; k++)
	{
		glm::vec4 d;
		for (int j = 0; j < retires2_FirstIntersectionPoint; j++) {
			d = -Derivative(params, object_a, object_b);
			float lastDerivativeLength = glm::dot(d, d);
			glm::vec4 lastParams = params;
			float B = 0;
			for (int i = 0; i < maxIterations_FirstIntersectionPoint  ; i++)
			{
				glm::vec4 currentDerivative = Derivative(params, object_a, object_b);
				float currentDerivativeLength = glm::dot(currentDerivative, currentDerivative);
				
				B = currentDerivativeLength / lastDerivativeLength;
				d = -currentDerivative + B * d;
				if (i % 20 == 0)
				{
					d = -currentDerivative;
					lastDerivativeLength = currentDerivativeLength;
				}
				//d = Clamp(d, object_a, object_b);

				// niby jest tutaj szukanie minimum α = min(F(α))
				// F(a) = f(params + a * d)
				float a = StepMinimalization(params, d, 0.0005f, 10, object_a, object_b);
				params = params + a * d;
				params = Clamp(params, object_a, object_b);
				lastDerivativeLength = currentDerivativeLength;


				if (glm::dot(d, d) < pow(epsilon, 2) )
					break;
				lastParams = params;
			}

			std::cout << std::endl << std::endl << Function(params, object_a, object_b) << std::endl << std::endl;
			if (Function(params, object_a, object_b) < epsilon) {
				break;
			}

			if (derivativeStop && glm::dot(d, d) < pow(epsilon, 2))
				break;

			params = RandomParamsCloseTo(startParametrs, (k + 1) * 0.05, object_a, object_b);
		}
		if (Function(params, object_a, object_b) < epsilon) {
			break;
		}

		if (derivativeStop && glm::dot(d, d) < epsilon)
			break;

	}
	return params;
}

std::pair<std::vector<glm::vec2>, std::vector<glm::vec2>> Intersection::IntersectionFrame(glm::vec4 firstIntersection, IntersectionAble* object_a, IntersectionAble* object_b, float step, float eps)
{
	std::vector<glm::vec2> params_a;
	params_a.push_back({ firstIntersection.x, firstIntersection.y }); 
	std::vector<glm::vec2> params_b;
	params_b.push_back({ firstIntersection.z, firstIntersection.w });
	auto cos = object_a->Parametrization(firstIntersection.x, firstIntersection.y);
	
	glm::vec4 params = firstIntersection;
	glm::vec4 lastParams = params;
	bool flip = false;
	for (int j = 0; j < 2; j++)
	{
		int i;
		for (i = 0; i < maxIterations_IntersectionFrame;
			i++) {
			params = NextIntersectionParams(params, object_a, object_b, step, eps, flip);

			for(int k = 0; k < 5 && Function(params, object_a, object_b) > eps; k++)
			{
				params = NextIntersectionParams(lastParams, object_a, object_b, step * powf(0.5, k + 1), eps, flip);
			}

			if (Function(params, object_a, object_b) > eps)
				break;

			lastParams = params;
			params_a.push_back({ params.x, params.y });
			params_b.push_back({ params.z, params.w });

			if (IsCreatedLoop(params_a, object_a))
				break;
		}

		if (IsCreatedLoop(params_a, object_a))
			break;
		std::reverse(params_a.begin(), params_a.end());
		std::reverse(params_b.begin(), params_b.end());
		params = firstIntersection;
		flip = !flip;
		
	}

	return { params_a, params_b };
}

glm::vec4 Intersection::NextIntersectionParams(glm::vec4 intersectionParams, IntersectionAble* object_a, IntersectionAble* object_b, float step, float eps, bool flipDirection)
{
	glm::vec3 normal_a = glm::cross(
		object_a->Derivative_v(intersectionParams.x, intersectionParams.y),
		object_a->Derivative_u(intersectionParams.x, intersectionParams.y));
	glm::vec3 normal_b = glm::cross(
		object_b->Derivative_v(intersectionParams.z, intersectionParams.w),
		object_b->Derivative_u(intersectionParams.z, intersectionParams.w));

	glm::vec3 t = glm::cross(normal_a, normal_b);
	t = glm::normalize(t);

	if (isnan(t.x) || isnan(t.y) || isnan(t.z))
		t = object_a->Derivative_v(intersectionParams.x, intersectionParams.y);

	if (flipDirection)
		t = -t;

	glm::vec3 intersectionPoint = object_a->Parametrization(intersectionParams.x, intersectionParams.y);

	glm::vec4 lastParams = intersectionParams;
	glm::vec4 params = intersectionParams;
	for (int i = 0; i < maxIterations_NextIntersectionParams; i++) {
		glm::vec3 p1 = object_a->Parametrization(params.x, params.y);
		glm::vec3 q1 = object_b->Parametrization(params.z, params.w);
		glm::vec4 f{ 
			p1 - q1,
			glm::dot( (p1 + q1)*0.5f - intersectionPoint, t) - step
		};
		
		glm::vec4 f_x{
			object_a->Derivative_v(params.x, params.y),
			glm::dot(object_a->Derivative_v(params.x, params.y), t) * 0.5f
		};
		glm::vec4 f_y{
			object_a->Derivative_u(params.x, params.y),
			glm::dot(object_a->Derivative_u(params.x, params.y), t) * 0.5f
		};
		glm::vec4 f_z{
			-object_b->Derivative_v(params.z, params.w),
			glm::dot(object_b->Derivative_v(params.z, params.w), t) * 0.5f
		};
		glm::vec4 f_w{
			-object_b->Derivative_u(params.z, params.w),
			glm::dot(object_b->Derivative_u(params.z, params.w), t) * 0.5f
		};

		glm::mat4 fd{ f_x, f_y, f_z, f_w };

		auto d = glm::inverse(fd) * f;
		float a = 1;//StepMinimalization(params, d, 0.1, 1, object_a, object_b);
		params = params - glm::inverse(fd) * f * a;
		
		params = Clamp(params, object_a, object_b);

		if (glm::dot(params - lastParams, params - lastParams) < eps)
			break;
		lastParams = params;
	}

	return params;
}



bool Intersection::IsIntersected(IntersectionAble* object_a, IntersectionAble* object_b) {
	return Function(FirstIntersectionPoint(object_a, object_b), object_a, object_b) < epsilon;
}

bool Intersection::IsIntersected(IntersectionAble* object_a, IntersectionAble* object_b, glm::vec4 params)
{
	return Function(params, object_a, object_b) < epsilon;
}

glm::mat4 Intersection::Hessian(const glm::vec4& params, IntersectionAble* object_a, IntersectionAble* object_b)
{
	CalculateObjects_derivative(params, object_a, object_b);
	glm::mat4 hessian{ 0.0f };
	hessian[0] = {
		 Derivative_xx(),
		 Derivative_xy(),
		 Derivative_xz(),
		 Derivative_xw()
	};

	hessian[1] = {
		 Derivative_yx(),
		 Derivative_yy(),
		 Derivative_yz(),
		 Derivative_yw()
	};

	hessian[2] = {
		 Derivative_zx(),
		 Derivative_zy(),
		 Derivative_zz(),
		 Derivative_zw()
	};

	hessian[3] = {
		 Derivative_wx(),
		 Derivative_wy(),
		 Derivative_wz(),
		 Derivative_ww()
	};

	return hessian;
}

inline glm::vec4 Intersection::Derivative(const glm::vec4& params, IntersectionAble* object_a, IntersectionAble* object_b)
{
	CalculateObjects_derivativeSingle(params, object_a, object_b);

	pos_a = object_a->Parametrization(params.x, params.y);
	a_d_v = object_a->Derivative_v(params.x, params.y);
	a_d_u = object_a->Derivative_u(params.x, params.y);

	pos_b = object_b->Parametrization(params.z, params.w);
	b_d_v = object_b->Derivative_v(params.z, params.w);
	b_d_u = object_b->Derivative_u(params.z, params.w);

	return {
		glm::dot(a_d_v, pos_a) - glm::dot(a_d_v, pos_b),
		glm::dot(a_d_u, pos_a) - glm::dot(a_d_u, pos_b),
		glm::dot(b_d_v, pos_b) - glm::dot(pos_a, b_d_v),
		glm::dot(b_d_u, pos_b) - glm::dot(pos_a, b_d_u)
	};
}

inline float Intersection::Function(const glm::vec4& params, IntersectionAble* object_a, IntersectionAble* object_b)
{
	glm::vec3 result = object_a->Parametrization(params.x, params.y) - object_b->Parametrization(params.z, params.w);
	return glm::dot(result, result);
}

glm::vec4 Intersection::ParamsCloseToPoint(glm::vec3 point, IntersectionAble* object_a, IntersectionAble* object_b)
{
	IntersectionConstFun constPoint{ point };
	auto params_1 = FirstIntersectionPoint(object_a, &constPoint, glm::vec4{ 0, 0, 0, 0 }, true);
	auto params_2 = FirstIntersectionPoint(object_b, &constPoint, glm::vec4{ 0, 0, 0, 0 }, true);
	return { params_1.x, params_1.y, params_2.x, params_2.y};
}

glm::vec4 Intersection::RandomTheClosetToEachOther(IntersectionAble* object_a, IntersectionAble* object_b)
{
	float distance = std::numeric_limits<float>::max();
	glm::vec4 params;

	for (int i = 0; i < randomTriesToFindMatch; i++) {
		glm::vec4 newParams = RandomParamsCloseTo({ 0, 0, 0, 0 }, 1, object_a, object_b);
		glm::vec3 pos_a = object_a->Parametrization(newParams.x, newParams.y);
		glm::vec3 pos_b = object_b->Parametrization(newParams.z, newParams.w);

		float newDistanse = glm::dot(pos_a - pos_b, pos_a - pos_b);
		if (newDistanse < distance)
		{
			params = newParams;
			distance = newDistanse;
		}
	}

	return params;
}

glm::vec4 Intersection::RandomTheClosetToPoint(glm::vec3 point, IntersectionAble* object_a, IntersectionAble* object_b)
{
	std::pair< glm::vec2, float > parms_length_a;
	std::pair< glm::vec2, float > parms_length_b;
	auto firstParams = RandomParamsCloseTo({ 0,0,0,0 }, 1, object_a, object_b);
	auto asigneFunction = [=](IntersectionAble* object, glm::vec2 parm) {
		std::pair< glm::vec2, float > parms_length;
		parms_length.first = { parm.x, parm.y };
		auto tmp = object->Parametrization(parm.x, parm.y);
		parms_length.second = glm::dot(tmp - point, tmp - point);
		return parms_length;
		};

	parms_length_a = asigneFunction(object_a, { firstParams.x, firstParams.y });
	parms_length_b = asigneFunction(object_b, { firstParams.z, firstParams.w });
	
	for (int i = 0; i < randomTriesToFindMatch; i++) {
		auto randomParams = RandomParamsCloseTo({ 0,0,0,0 }, 1, object_a, object_b);
		auto paramsLength = asigneFunction(object_a, { randomParams.x, randomParams.y });
		if (paramsLength.second < parms_length_a.second)
			parms_length_a = paramsLength;

		paramsLength = asigneFunction(object_b, { randomParams.z, randomParams.w });
		if (paramsLength.second < parms_length_b.second)
			parms_length_b = paramsLength;
	}

	return {parms_length_a.first, parms_length_b.first};
}

glm::vec4 Intersection::TheFaresParams(IntersectionAble* object_a)
{
	auto field_v = object_a->Field_v();
	auto field_u = object_a->Field_v();

	float v_start = field_v.x;
	float u_start = field_u.x;

	float v_end = field_v.y;
	float u_end = field_u.y;
	if (object_a->CanWrap().x)
		v_end = v_start + (field_v.y - field_v.x) * 0.5;
	if (object_a->CanWrap().y)
		u_end = u_start + (field_u.y - field_u.x) * 0.5;

	return {v_start, u_start, v_end, u_end };
}

inline void Intersection::CalculateObjects_derivative(const glm::vec4& params, IntersectionAble* object_a, IntersectionAble* object_b)
{

	pos_a = object_a->Parametrization(params.x, params.y);
	pos_b = object_b->Parametrization(params.z, params.w);

	a_d_v = object_a->Derivative_v(params.x, params.y);
	a_d_u = object_a->Derivative_u(params.x, params.y);

	a_d_vv = object_a->Derivative_vv(params.x, params.y);
	a_d_uu = object_a->Derivative_uu(params.x, params.y);

	a_d_vu = object_a->Derivative_vu(params.x, params.y);
	a_d_uv = object_a->Derivative_uv(params.x, params.y);

	b_d_v = object_b->Derivative_v(params.z, params.w);
	b_d_u = object_b->Derivative_u(params.z, params.w);
	b_d_vv = object_b->Derivative_vv(params.z, params.w);
	b_d_uu = object_b->Derivative_uu(params.z, params.w);

	b_d_vu = object_b->Derivative_vu(params.z, params.w);
	b_d_uv = object_b->Derivative_uv(params.z, params.w);
}

inline void Intersection::CalculateObjects_derivativeSingle(const glm::vec4& params, IntersectionAble* object_a, IntersectionAble* object_b)
{
	pos_a = object_a->Parametrization(params.x, params.y);
	pos_b = object_b->Parametrization(params.z, params.w);

	a_d_v = object_a->Derivative_v(params.x, params.y);
	a_d_u = object_a->Derivative_u(params.x, params.y);

	b_d_v = object_b->Derivative_v(params.z, params.w);
	b_d_u = object_b->Derivative_u(params.z, params.w);
}

inline float Intersection::StepMinimalization(const glm::vec4& params, const glm::vec4& d, float min, float max, IntersectionAble* object_a, IntersectionAble* object_b)
{
	float x_1 = max - c * (max - min);
	float x_2 = min + c * (max - min);

	for (int i = 0; i < 20; i++) {		

		glm::vec4 newParams_x1 = params + x_1 * d;
		glm::vec4 newParams_x2 = params + x_2 * d;
		bool clamped = false;
		newParams_x1 = Clamp(newParams_x1, object_a, object_b, clamped);
		if (clamped)
		{
			int j = 0;
			while (clamped)
			{
				min *= 0.5f;
				newParams_x1 = params + min * d;
				Clamp(newParams_x1, object_a, object_b, clamped);
				if (j > 10)
					return 0;
				j++;
			}
			return min;
		}

		clamped = false;
		Clamp(newParams_x2, object_a, object_b, clamped);
		int k = 0;
		while (clamped)
		{
			x_2 = (x_1 + x_2) * 0.5;
			newParams_x2 = params + x_2 * d;
			Clamp(newParams_x2, object_a, object_b, clamped);
			if (k > 10)
				return min;
			k++;
		}
		newParams_x2 = Clamp(newParams_x2, object_a, object_b, clamped);

		/*if (Clamp(newParams_x1, object_a, object_b) != newParams_x1 ||
			Clamp(newParams_x2, object_a, object_b) != newParams_x2) {
			return Function(params + min * d, object_a, object_b) < Function(params + min * d, object_a, object_b)? min: max;
		}*/

		float f_1 = Function(newParams_x1, object_a, object_b);
		float f_2 = Function(newParams_x2, object_a, object_b);

		if (f_2 > f_1) {
			// min = min;
			max = x_2;
			x_2 = x_1;
			x_1 = max - c * (max - min);
		}
		else {
			min = x_1;
			//max = max;
			x_1 = x_2;
			x_2 = min + c * (max - min);
		}

		if (abs(max - min) < epsilon)
			break;
	}

	return (max + min) * 0.5f;
}

float  Intersection::Derivative_x() {
	return glm::dot(a_d_v, pos_a) -
		glm::dot(a_d_v, pos_b);
 }
float  Intersection::Derivative_y(){
	return glm::dot(a_d_u, pos_a) -
		glm::dot(a_d_u, pos_b);
}
float  Intersection::Derivative_z() {
	return glm::dot(b_d_v, pos_b) -
		glm::dot(pos_a, b_d_v);
}
float  Intersection::Derivative_w(){
	return glm::dot(b_d_u, pos_b) -
		glm::dot(pos_a, b_d_u);
}



float  Intersection::Derivative_xx(){
	return 2 * (glm::dot(a_d_vv, pos_a) +
		glm::dot(a_d_v, a_d_v)) -
		glm::dot(a_d_vv, pos_b);
}
float  Intersection::Derivative_xy(){
	return 2 * (glm::dot(a_d_vu, pos_a) +
		glm::dot(a_d_v, a_d_u)) -
		glm::dot(a_d_vu, pos_b);
}
float  Intersection::Derivative_xz(){
	return -glm::dot(a_d_v, b_d_v);
}
float  Intersection::Derivative_xw() {
	return -glm::dot(a_d_v, b_d_u);
}



float  Intersection::Derivative_yx(){
	return 2 * (glm::dot(a_d_uv, pos_a) +
		glm::dot(a_d_v, a_d_u)) -
		glm::dot(a_d_uv, pos_b);
}
float  Intersection::Derivative_yy() {
	return 2 * (glm::dot(a_d_uu, pos_a) +
		glm::dot(a_d_v, a_d_u)) -
		glm::dot(a_d_uu, pos_b);
}
float  Intersection::Derivative_yz() {
	return -glm::dot(a_d_u, b_d_v);
}
float  Intersection::Derivative_yw() {
	return -glm::dot(a_d_u, b_d_u);
}


	 
float  Intersection::Derivative_zx() {
	return -glm::dot(a_d_v, b_d_v);
}
float  Intersection::Derivative_zy() {
	return -glm::dot(a_d_u, b_d_u);
}
float  Intersection::Derivative_zz(){
	return 2 * (glm::dot(b_d_vv, pos_b) +
		glm::dot(b_d_v, b_d_v)) -
		glm::dot(pos_a, b_d_vv);
}
float  Intersection::Derivative_zw(){
	return 2 * (glm::dot(b_d_vu, pos_b) +
		glm::dot(b_d_v, b_d_u)) -
		glm::dot(pos_a, b_d_vu);
}



float  Intersection::Derivative_wx() {
	return -glm::dot(a_d_v, b_d_u);
}
float  Intersection::Derivative_wy() {
	return -glm::dot(a_d_u, b_d_u);
}
float  Intersection::Derivative_wz(){
	return 2 * (glm::dot(b_d_uv, pos_b) +
		glm::dot(b_d_v, b_d_u)) -
		glm::dot(pos_a, b_d_uv);
}
float  Intersection::Derivative_ww(){
	return 2 * (glm::dot(b_d_uu, pos_b) +
		glm::dot(b_d_v, b_d_u)) -
		glm::dot(pos_a, b_d_uu);
}

inline glm::vec4 Intersection::RandomParamsCloseTo(glm::vec4 params, float length, IntersectionAble* object_a, IntersectionAble* object_b)
{
	if (length > 1)
		length = 1;
	if (length < 0)
		length = 0;
	std::random_device rd;
	std::mt19937 mt(rd());
	auto field_a_v = object_a->Field_v();
	auto field_a_u = object_a->Field_u();
	auto field_b_v = object_b->Field_v();
	auto field_b_u = object_b->Field_u();

	glm::vec4 start{ field_a_v.x, field_a_u.x, field_b_v.x, field_b_u.x };
	glm::vec4 end{ field_a_v.y, field_a_u.y, field_b_v.y, field_b_u.y };

	start = params - length * (params - start);
	end = params + length * (end - params);
	std::uniform_real_distribution<float> dist_x{ start.x, end.x };
	std::uniform_real_distribution<float> dist_y{ start.y, end.y };
	std::uniform_real_distribution<float> dist_z{ start.z, end.z };
	std::uniform_real_distribution<float> dist_w{ start.w, end.w };

	return { 
		dist_x(mt),
		dist_y(mt),
		dist_z(mt),
		dist_w(mt) };
}

inline glm::vec4 Intersection::Clamp(glm::vec4 params, IntersectionAble* object_a, IntersectionAble* object_b)
{
	bool clamped = false;
	auto size_a_v = object_a->Field_v();
	auto size_a_u = object_a->Field_u();

	auto size_b_v = object_b->Field_v();
	auto size_b_u = object_b->Field_u();

	auto canWrap_a = object_a->CanWrap();
	auto canWrap_b = object_b->CanWrap();

	if (!canWrap_a.x) {
		params.x = Clamp(params.x, size_a_v.x, size_a_v.y);
	}
	else {
		params.x = Wrap(params.x, size_a_v.x, size_a_v.y);
	}
		///
	if (!canWrap_a.y) {
		params.y = Clamp(params.y, size_a_u.x, size_a_u.y);
	}
	else {
		params.y = Wrap(params.y, size_a_u.x, size_a_u.y);
	}
	////
	if (!canWrap_b.x) {
		params.z = Clamp(params.z, size_b_v.x, size_b_v.y);
	}
	else {
		params.z = Wrap(params.z, size_b_v.x, size_b_v.y);
	}
	///
	if (!canWrap_b.y) {
		params.w = Clamp(params.w, size_b_u.x, size_b_u.y);
	}
	else {
		params.w = Wrap(params.w, size_b_u.x, size_b_u.y);
	}


	return params;
}

inline glm::vec4 Intersection::Clamp(glm::vec4 params, IntersectionAble* object_a, IntersectionAble* object_b, bool& clamped)
{
	auto result = Clamp(params, object_a, object_b);

	auto canWrap_a = object_a->CanWrap();
	auto canWrap_b = object_b->CanWrap();

	clamped = 
		(!canWrap_a.x && result.x != params.x) ||
		(!canWrap_a.y && result.y != params.y) ||
		(!canWrap_b.x && result.z != params.z) ||
		(!canWrap_b.y && result.w != params.w);
	return result;
}

inline float Intersection::Clamp(const float& a, const float& min, const float& max)
{
	if (a < min)
		return min;

	if (a > max)
		return max;

	return a;
}

inline float Intersection::Wrap(const float& a, const float& min, const float& max)
{
	float x = a - min;
	float length = max - min;
	
	if (x < 0)
	{
		x = -x;
		x = std::fmod(x, length);
		x = length - x;
	}
	else
		x = std::fmod(x, length);

	return x + min;
}

inline bool Intersection::IsCreatedLoop(const std::vector<glm::vec2>& points, IntersectionAble* object)
{
	if (points.size() > 3) {
		glm::vec3 p0 = object->Parametrization(points[0].x, points[0].y);
		glm::vec3 p1 = object->Parametrization(points[1].x, points[1].y);
		glm::vec3 p2 = object->Parametrization(points[points.size() - 1].x, points[points.size() - 1].y);

		glm::vec3 a = p1 - p0;
		glm::vec3 b = p2 - p1;
		glm::vec3 c = p0 - p2;
		if (glm::dot(b, b) + glm::dot(c, c) < glm::dot(a, a))
			return true;
	}
	return false;
}

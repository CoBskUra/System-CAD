#include "BezierSurfaceC2.h"
BezierSurfaceC2::BezierSurfaceC2(const char* name) : BezierSurfaceC2(name, FigureType::BezierSurfaceC2)
{
}



BezierSurfaceC2::BezierSurfaceC2() : BezierSurfaceC2("BezierSurfaceC2")
{
}


BezierSurfaceC2::BezierSurfaceC2(const char* name, FigureType type): 
	BezierSurface(name, type, StaticShaders::GetBezierSurfaceC2(), StaticShaders::GetBezierCurve())
{
	showBezierCurve = false;
	CreateBezierVAO();
	SetUnmarkColor(glm::vec4(1, 1, 0, 1));
}

glm::vec3 BezierSurfaceC2::GeneratePosForVertexInPatch(int verticalID, int horizontalID, int k1, int k2)
{
	float width = (2 + horizontalNum);
	float patchNumVertical = (2 + verticalNum);
	float alfa = 0;
	float scalar = 1;

	glm::vec3 surPos = glm::vec3{ horizontalID , 0,  verticalID } + glm::vec3{ k2, 0, k1 } - glm::vec3{ width / 2.0f, 0, patchNumVertical / 2.0f };
	//return surPos;
	switch (creationType)
	{
	case BezierSurfaceC2::CreationType::surface:
		return surPos;
		break;
	case BezierSurfaceC2::CreationType::cylinder:
		if (creationType == CreationType::cylinder &&
			horizontalID >= horizontalNum - 3 && k2 >= horizontalNum - horizontalID)
		{
			/*k2 = horizontalID;
			horizontalID = k2;*/
			//k2--;
			horizontalID += k2;
			alfa = (horizontalID)*M_PI * 2 / ((float)horizontalNum );
		}
		else
			alfa = (horizontalID)*M_PI * 2 / ((float)horizontalNum );

		surPos.z *= 1.0f / patchNumVertical * this->height;

		surPos.x = cosf(alfa) * radius;
		surPos.y = sinf(alfa) * radius;

		/*alfa = M_PI * 2 / (float)horizontalNum;

		surPos.x = radius;
		if (k2 == 1) {
			surPos.y = 2.0f / 3.0f * tanf(alfa * 0.5f) * radius;
		}
		else if (k2 == 2) {
			surPos.y = -2.0f / 3.0f * tanf(alfa * 0.5f) * radius;
			surPos = MathOperations::RotationAlongAxis(surPos, alfa, glm::vec3{ 0, 0, 1 });
		}
		else if (k2 == 3) {
			surPos = MathOperations::RotationAlongAxis(surPos, alfa, glm::vec3{ 0, 0, 1 });
		}

		surPos = MathOperations::RotationAlongAxis(surPos, alfa * horizontalID, glm::vec3{ 0, 0, 1 });*/

		return surPos;
		break;
	default:
		break;
	}

	return glm::vec3{ horizontalID * 3, 0,  verticalID * 3 } + glm::vec3{ k2, 0, k1 };;
}

std::vector<glm::vec3> BezierSurfaceC2::DeBoreBaiseToBezier(std::vector<glm::vec3>& points)
{
	std::vector<glm::vec3> bezierBase;
	for (int y = 0; 4 * y < points.size(); y++) {
		bezierBase.push_back((0.5f * points[0 + y * 4] + 2.0f * points[1 + y * 4] + 0.5f * points[2 + y * 4]) / 3.0f);
		bezierBase.push_back((2.0f * points[1 + y * 4] + 1.0f * points[2 + y * 4]) / 3.0f);
		bezierBase.push_back((1.0f * points[1 + y * 4] + 2.0f * points[2 + y * 4]) / 3.0f);
		bezierBase.push_back((0.5f * points[1 + y * 4] + 2.0f * points[2 + y * 4] + 0.5f * points[3 + y * 4] )/ 3.0f);
	}
	return bezierBase;
}

BezierSurfaceC2::~BezierSurfaceC2()
{
	for (int i = 0; i < controlPoints.size(); i++)
	{
		if (!controlPoints[i].get())
			continue;
		controlPoints[i]->RemoveOwner();
		controlPoints[i]->EraseContainer(this);
		controlPoints[i].reset();
	}
}


Figure* BezierSurfaceC2::TakePoint(int verticalID, int horizontalID, int k1, int k2) const
{
	auto p = controlPoints[TakeId(verticalID, horizontalID, k1, k2)];
	return p.get();
}

int BezierSurfaceC2::TakeId(int verticalID, int horizontalID, int k1, int k2) const
{
	if (creationType == CreationType::cylinder &&
		horizontalID >= horizontalNum - 3 && k2 >= horizontalNum - horizontalID)
	{
		k2 = k2 - (horizontalNum - horizontalID);
		horizontalID = 0;
	}

	int width = (3 + horizontalNum);
	int id = (verticalID + k1) * width + k2 + horizontalID;
	return id;
}

int BezierSurfaceC2::MaxSize()
{
	int width = (4 + (horizontalNum - 1) * 3);
	int height = (4 + (verticalNum - 1) * 3);

	return width * height;
}


BezierSurfaceC2::BezierSurfaceC2(MG1::BezierSurfaceC2 bs2, Scene* scene, int idOffset) : BezierSurfaceC2()
{
	if (bs2.name != "")
		this->SetName(bs2.name.c_str());
	this->SetId(bs2.GetId() + idOffset);

	refrenceScene = scene;
	this->verticalNum = bs2.size.y;
	this->horizontalNum = bs2.size.x;
	/*if (verticalNum > horizontalNum) {
		std::swap(verticalNum, horizontalNum);
	}*/
	this->creationType = bs2.uWrapped ? CreationType::cylinder : CreationType::surface;
	controlPoints.resize(MaxSize());
	TurnOffStartupInterfers();
	numberOfVertexes = 0;
	canAdd = true;

	for (int i = 0; i < verticalNum; i++) {
		for (int j = 0; j < horizontalNum; j++) {
			for (int k1 = 0; k1 < 4; k1++) {
				for (int k2 = 0; k2 < 4; k2++) {
					int pointId = bs2.patches.at(i * horizontalNum + j).controlPoints.at(k1 * 4 + k2).GetId();
					auto p = scene->byID(pointId + idOffset);
					p->SetObjectOwner(this);
					int id = TakeId(i, j, k1, k2);
					controlPoints[id] = std::static_pointer_cast<Point>(p);
					Add(p.get());
					numberOfVertexes++;
				}
			}
		}
	}
	canAdd = false;

	CreateBezierVAO();
}

MG1::BezierSurfaceC2 BezierSurfaceC2::Serialize(int idOffset) const
{
	MG1::BezierSurfaceC2 bs2{};
	bs2.SetId(GetId() - idOffset);
	bs2.name = this->name;
	bs2.size.x = horizontalNum; bs2.size.y = verticalNum;

	bs2.uWrapped = creationType == CreationType::cylinder;
	bs2.vWrapped = !bs2.uWrapped;

	for (int i = 0; i < verticalNum; i++) {
		for (int j = 0; j < horizontalNum; j++) {
			MG1::BezierPatchC2 patch{};
			for (int k1 = 0; k1 < 4; k1++) {
				for (int k2 = 0; k2 < 4; k2++) {
					MG1::PointRef pointRef{ TakePoint(i, j, k1, k2)->GetId() - idOffset };
					patch.controlPoints.push_back(pointRef);					
				}
			}
			bs2.patches.push_back(patch);
		}
	}

	return bs2;
}

glm::vec3 BezierSurfaceC2::Parametrization(int patchV, int patchH, float v, float u)
{
	auto points =  ControlPointsPosVector(patchV, patchH);
	points = DeBoreBaiseToBezier(points);
	
	// do napisania jeszcze na razie testuje torusy ale kompilator mi krzyczy
	std::vector<glm::vec3> bezierPoints;
	bezierPoints.reserve(4);
	for (int y = 0; y < 4; y++) {
		bezierPoints.push_back(MathOperations::Bezier3D(v,
			points[0 + y*4],
			points[1 + y * 4],
			points[2 + y * 4],
			points[3 + y * 4]));
	}

	bezierPoints = DeBoreBaiseToBezier(bezierPoints);
	glm::vec3 p = MathOperations::Bezier3D(u,
		bezierPoints[0],
		bezierPoints[1],
		bezierPoints[2],
		bezierPoints[3]
	);

	return p;
}

glm::vec3 BezierSurfaceC2::DerivativeV(int patchV, int patchH, float v, float u)
{
	auto points = ControlPointsPosVector(patchV, patchH);
	points = DeBoreBaiseToBezier(points);

	// do napisania jeszcze na razie testuje torusy ale kompilator mi krzyczy
	std::vector<glm::vec3> bezierPoints;
	bezierPoints.reserve(4);
	for (int y = 0; y < 4; y++) {
		bezierPoints.push_back( MathOperations::Bezier3D_derivative(v,
			points[0 + y * 4],
			points[1 + y * 4],
			points[2 + y * 4],
			points[3 + y * 4]));
	}

	bezierPoints = DeBoreBaiseToBezier(bezierPoints);
	glm::vec3 p = MathOperations::Bezier3D(u,
		bezierPoints[0],
		bezierPoints[1],
		bezierPoints[2],
		bezierPoints[3]
	);

	return p;
	//float h = 0.00001;
	//if (patchV + v <= SurfaceSize().x - h)
	//{
	//	return (Parametrization(patchV, patchH, v + h, u) - Parametrization(patchV, patchH, v, u)) / h;
	//}
	//else
	//	return { 0,0,0 };

}

glm::vec3 BezierSurfaceC2::DerivativeU(int patchV, int patchH, float v, float u)
{
	auto points = ControlPointsPosVector(patchV, patchH);
	points = DeBoreBaiseToBezier(points);

	// do napisania jeszcze na razie testuje torusy ale kompilator mi krzyczy
	std::vector<glm::vec3> bezierPoints;
	bezierPoints.reserve(4);
	for (int y = 0; y < 4; y++) {
		bezierPoints.push_back( MathOperations::Bezier3D(v,
			points[0 + y * 4],
			points[1 + y * 4],
			points[2 + y * 4],
			points[3 + y * 4]));
	}

	bezierPoints = DeBoreBaiseToBezier(bezierPoints);
	glm::vec3 p = MathOperations::Bezier3D_derivative(u,
		bezierPoints[0],
		bezierPoints[1],
		bezierPoints[2],
		bezierPoints[3]
	);

	return p;

	/*float h = 0.00001;
	if (patchH + u <= SurfaceSize().y - h)
	{
		return (Parametrization(patchV, patchH, v, u + h) - Parametrization(patchV, patchH, v, u )) / h;
	}
	else if(creationType == CreationType::cylinder)
		return (Parametrization(patchV, 0, v, h - (1 - u)) - Parametrization(patchV, patchH, v, u)) / h;
	else
		return { 0,0,0 };*/
}

glm::vec3 BezierSurfaceC2::DerivativeVU(int patchV, int patchH, float v, float u)
{
	auto points = ControlPointsPosVector(patchV, patchH);
	points = DeBoreBaiseToBezier(points);

	// do napisania jeszcze na razie testuje torusy ale kompilator mi krzyczy
	std::vector<glm::vec3> bezierPoints;
	bezierPoints.reserve(4);
	for (int y = 0; y < 4; y++) {
		bezierPoints.push_back(MathOperations::Bezier3D_derivative(v,
			points[0 + y * 4],
			points[1 + y * 4],
			points[2 + y * 4],
			points[3 + y * 4]));
	}

	bezierPoints = DeBoreBaiseToBezier(bezierPoints);
	glm::vec3 p = MathOperations::Bezier3D_derivative(u,
		bezierPoints[0],
		bezierPoints[1],
		bezierPoints[2],
		bezierPoints[3]
	);

	return p;
}

glm::vec3 BezierSurfaceC2::DerivativeUU(int patchV, int patchH, float v, float u)
{
	auto points = ControlPointsPosVector(patchV, patchH);
	points = DeBoreBaiseToBezier(points);

	// do napisania jeszcze na razie testuje torusy ale kompilator mi krzyczy
	std::vector<glm::vec3> bezierPoints;
	bezierPoints.reserve(4);
	for (int y = 0; y < 4; y++) {
		bezierPoints.push_back( MathOperations::Bezier3D_derivative(v,
			points[0 + y * 4],
			points[1 + y * 4],
			points[2 + y * 4],
			points[3 + y * 4]));
	}

	bezierPoints = DeBoreBaiseToBezier(bezierPoints);
	glm::vec3 p = MathOperations::Bezier3D_derivative(u,
		bezierPoints[0],
		bezierPoints[1],
		bezierPoints[2],
		bezierPoints[3]
	);

	return p;
}

glm::vec3 BezierSurfaceC2::DerivativeVV(int patchV, int pathH, float v, float u)
{
	// do napisania jeszcze na razie testuje torusy ale kompilator mi krzyczy
	throw "cos";
	return glm::vec3();
}

glm::vec3 BezierSurfaceC2::DerivativeUV(int patchV, int pathH, float v, float u)
{
	// do napisania jeszcze na razie testuje torusy ale kompilator mi krzyczy
	throw "cos";
	return glm::vec3();
}

glm::bvec2 BezierSurfaceC2::CanWrap()
{
	return { false, creationType == CreationType::cylinder };
}



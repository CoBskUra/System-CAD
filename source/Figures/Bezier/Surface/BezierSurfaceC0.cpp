#include "BezierSurfaceC0.h"
#include <imgui_impl_opengl3.h>

BezierSurfaceC0::BezierSurfaceC0(const char* name) : BezierSurfaceC0(name, 
	FigureType::BezierSurfaceC0)
{
}

BezierSurfaceC0::BezierSurfaceC0(MG1::BezierSurfaceC0 bs2, Scene* scene, int idOffset) : BezierSurfaceC0()
{
	if (bs2.name != "")
		this->SetName(bs2.name.c_str());
	this->SetId(bs2.GetId() + idOffset);

	refrenceScene = scene;
	this->verticalNum = bs2.size.y;
	this->horizontalNum = bs2.size.x;
	/*if (verticalNum < horizontalNum) {
		std::swap(verticalNum, horizontalNum);
	}*/
	this->creationType = bs2.vWrapped ? CreationType::cylinder : CreationType::surface;
	controlPoints.resize(MaxSize());
	TurnOffStartupInterfers();
	numberOfVertexes = 0;

	for (int i = 0; i < verticalNum; i++) {
		for (int j = 0; j < horizontalNum; j++) {
			for (int k1 = 0; k1 < 4; k1++) {
				for (int k2 = 0; k2 < 4; k2++) {
					int cosId = bs2.patches.at(i * horizontalNum + j).controlPoints.at(k1 * 4 + k2).GetId();
					auto p = scene->byID(bs2.patches.at(i * horizontalNum + j).controlPoints.at(k1 * 4 + k2).GetId() + idOffset);
					p->SetObjectOwner(this);
					int id = TakeId(i, j, k1, k2);
					controlPoints[id] = std::static_pointer_cast<Point>(p);
					Add(p.get());
					numberOfVertexes++;
				}
			}
		}
	}

	CreateBezierVAO();
}

BezierSurfaceC0::BezierSurfaceC0() : BezierSurfaceC0("BezierSurfaceC0")
{
}


BezierSurfaceC0::BezierSurfaceC0(const char* name, FigureType type):
	BezierSurface(name, type, StaticShaders::GetBezierSurfaceC0(), StaticShaders::GetBezierCurve())
{
	showBezierCurve = false;
	CreateBezierVAO();
	SetUnmarkColor(glm::vec4(1, 1, 0, 1));
}

std::vector<glm::mat<4, 4, float>> BezierSurfaceC0::ControlPointsMatrix(int patchV, int patchH)
{
	glm::mat<4, 4, float> points_x{}, points_y{}, points_z{};
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			glm::vec3 pos = TakePoint(patchV, patchH, i, j)->transpose->GetPosition();
			points_x[j][i] = pos.x;
			points_y[j][i] = pos.y;
			points_z[j][i] = pos.z;
		}
	}
	return { points_x, points_y, points_z };
}

BezierSurfaceC0::~BezierSurfaceC0()
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

MG1::BezierSurfaceC0 BezierSurfaceC0::Serialize(int idOffset) const
{
	MG1::BezierSurfaceC0 bs0{};
	bs0.SetId(GetId() - idOffset);
	bs0.name = name;
	bs0.size.x = horizontalNum; bs0.size.y = verticalNum;

	bs0.vWrapped = creationType == CreationType::cylinder;

	for (int i = 0; i < verticalNum; i++) {
		for (int j = 0; j < horizontalNum; j++) {
			MG1::BezierPatchC0 patch{};
			for (int k1 = 0; k1 < 4; k1++) {
				for (int k2 = 0; k2 < 4; k2++) {
					MG1::PointRef pointRef{ TakePoint(i, j, k1, k2)->GetId() - idOffset };
					patch.controlPoints.push_back(pointRef);
				}
			}
			bs0.patches.push_back(patch);

		}
	}
	return bs0;
}



Figure* BezierSurfaceC0::TakePoint(int verticalID, int horizontalID, int k1, int k2) const
{
	auto p = controlPoints[TakeId(verticalID, horizontalID, k1, k2)];
	return p.get();
}

int BezierSurfaceC0::TakeId(int verticalID, int horizontalID, int k1, int k2) const
{
	if (creationType == CreationType::cylinder &&
		horizontalID == horizontalNum - 1 && k2 == 3) {
		k2 = 0;
		horizontalID = 0;
	}

	int width = (4 + (horizontalNum - 1) * 3);
	int id = (verticalID * 3 + k1) * width + k2 + horizontalID * 3;
	return id;
}

glm::vec4 BezierSurfaceC0::Bernstain3D(float t) {

	glm::vec4 bernstain;
	bernstain.x = powf(1 - t, 3);
	bernstain.y = 3 * powf(1 - t, 2)*t;
	bernstain.z = 3 * 1 - t*powf(t, 2);
	bernstain.w = powf(t, 3);


	return  bernstain;
}

glm::vec4 BezierSurfaceC0::Bernstain3DDerivative(float t)
{
	glm::vec3 bern2D = Bernstain2D(t);
	 
	return glm::vec4{-3* bern2D.x, 3 * (bern2D.x - bern2D.y), 3 * (bern2D.y - bern2D.z), 3* bern2D.z };
}

glm::vec3 BezierSurfaceC0::Bernstain2D(float t)
{
	glm::vec3 bernstain;
	bernstain.x = powf(1 - t, 2);
	bernstain.y = 2 * (1 - t )* t;
	bernstain.z = powf(t, 2);


	return  bernstain;
}

glm::vec3 BezierSurfaceC0::BernstainPolinomalDe(float t)
{

	return glm::vec3();
}

glm::vec3 BezierSurfaceC0::DerivativeV(int patchV, int patchH, float v, float u)
{
	auto controlPoints = ControlPointsMatrix(patchV, patchH);

	auto berU = Bernstain3D(u);
	auto berV = Bernstain3DDerivative(v);
	glm::vec3 result;
	for (int i = 0; i < 3; i++)
		result[i] = glm::dot(berV, controlPoints[i] * berU);

	return result;
}

glm::vec3 BezierSurfaceC0::DerivativeU(int patchV, int patchH, float v, float u)
{
	auto controlPoints = ControlPointsMatrix(patchV, patchH);

	auto berU = Bernstain3DDerivative(u);
	auto berV = Bernstain3D(v);
	glm::vec3 result;
	for (int i = 0; i < 3; i++)
		result[i] = glm::dot(berV, controlPoints[i] * berU);

	return result;
}

glm::vec3 BezierSurfaceC0::DerivativeVU(int patchV, int patchH, float v, float u)
{
	auto controlPoints = ControlPointsMatrix(patchV, patchH);

	auto berU = Bernstain3DDerivative(u);
	auto berV = Bernstain3DDerivative(v);
	glm::vec3 result;
	for (int i = 0; i < 3; i++)
		result[i] = glm::dot(berV, controlPoints[i] * berU);

	return result;
}

int BezierSurfaceC0::MaxSize()
{
	int width = (4 + (horizontalNum - 1) * 3);
	int height = (4 + (verticalNum - 1) * 3);
	return width * height;
}

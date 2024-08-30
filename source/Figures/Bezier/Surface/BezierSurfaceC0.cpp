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

	CreateBezier();
}

BezierSurfaceC0::BezierSurfaceC0() : BezierSurfaceC0("BezierSurfaceC0")
{
}


BezierSurfaceC0::BezierSurfaceC0(const char* name, FigureType type): BezierSurface(name, type)
{
	showBezierCurve = false;
	CreateBezier();
	SetUnmarkColor(glm::vec4(1, 1, 0, 1));


	shader = StaticShaders::GetPointerToBezierSurfaceC0();
	shader_curve = StaticShaders::GetPointerToBezierCurve();
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

int BezierSurfaceC0::MaxSize()
{
	int width = (4 + (horizontalNum - 1) * 3);
	int height = (4 + (verticalNum - 1) * 3);
	return width * height;
}

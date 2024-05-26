#include "BezierC2.h"

BezierC2::BezierC2( const char* name):BezierC2(name, "##BezierC2", FigureType::BezierC2)
{

}

BezierC2::BezierC2():BezierC2("BezierC2")
{
}


BezierC2::~BezierC2()
{
	for (int i = 0; i < virtualPoints.size(); i++)
		delete virtualPoints[i];
}

void BezierC2::Draw(GLFWwindow* window, const Camera& camera)
{
	if(IsSomethingChange())
		Update();
	if (showVirtualPoints) {
		for (int i = 0; i < virtualPointsNumber; i++)
			virtualPoints.at(i)->Draw(window, camera);
	}
	innerBezierC0.Draw(window, camera);
}

void BezierC2::ActiveImGui()
{
	ImGui::PushID(id);

	if (ImGui::RadioButton("Virtual Points Interfers", showVirtualPointsInterfers))
		showVirtualPointsInterfers = !showVirtualPointsInterfers;
	ImGui::SameLine();
	if (ImGui::RadioButton("Show Virtual Points on sccene", showVirtualPoints))
		showVirtualPoints = !showVirtualPoints;

	BezierBase::ActiveImGui();

	if (showVirtualPointsInterfers) {
		for (int i = 0; i < virtualPointsNumber; i++)
		{
			ImGui::PushID(i);
			virtualPoints[i]->ActiveImGui();
			ImGui::PopID();
		}
	}
	ImGui::PopID();
}


bool BezierC2::Inputs(GLFWwindow* window, const Camera& camera)
{
	bool any = false;
	if (showVirtualPoints) {
		for (int i = 0; i < virtualPointsNumber; i++)
		{
			if (virtualPoints.at(i)->Inputs(window, camera)) {
				any = true;
				break;
			}
		}
	}
	return any;
}

BezierC2::BezierC2(const char* name, const char* uniqueName, FigureType type)
	:BezierBase(name, uniqueName, type)
{}

void BezierC2::ChangeShowBezierPol()
{
	innerBezierC0.ChangeShowBezierPol();
}

void BezierC2::CreateBezier()
{
	innerBezierC0.Clear();
	virtualPointsNumber = 0;
	if (ContainerSize() < 3)
		return;

	if ((ContainerSize() - 3) * 3 + 4 > virtualPoints.size())
	{
		for (int i = virtualPoints.size(); virtualPoints.size() < (ContainerSize() - 3) * 3 + 4; i++) {
			VirtualPoint* p = new VirtualPoint();
			p->SetColor(virtualPointColor);
			virtualPoints.push_back(p);
			p->SetName(std::to_string(i).c_str());
			p->SetObjectOwner(this);
		}
	}
	
	auto p = virtualPoints.at(virtualPointsNumber);
	virtualPointsNumber++;
	p->connectedPoints.clear();
	p->transpose->SetObjectPosition(
		(At(0)->transpose->GetPosition() * 2.0f + At(1)->transpose->GetPosition()) / 3.0f
	);

	p->connectedPoints.push_back(At(0));
	p->connectedPoints.push_back(At(1));

	innerBezierC0.Add(p);

	for (int i = 1; i < ContainerSize() - 1; i++) {
		auto f = virtualPoints.at(virtualPointsNumber);
		virtualPointsNumber++;
		f->connectedPoints.clear();

		auto g = virtualPoints.at(virtualPointsNumber);
		g->connectedPoints.clear();
		virtualPointsNumber++;

		f->transpose->SetObjectPosition(
			(At(i)->transpose->GetPosition() * 2.0f + At(i + 1)->transpose->GetPosition() * 1.0f)/3.0f);
		g->transpose->SetObjectPosition(
			(At(i)->transpose->GetPosition() * 1.0f + At(i + 1)->transpose->GetPosition() * 2.0f) / 3.0f);

		innerBezierC0.Add(f);
		innerBezierC0.Add(g);

		f->connectedPoints.push_back(At(i));
		f->connectedPoints.push_back(At(i + 1));

		g->connectedPoints.push_back(At(i));
		g->connectedPoints.push_back(At(i + 1));

		if (i + 2 < ContainerSize()) {
			glm::vec3 posNextF = (At(i + 1)->transpose->GetPosition() * 2.0f + At(i + 2)->transpose->GetPosition() * 1.0f) / 3.0f;
			auto e = virtualPoints.at(virtualPointsNumber);
			virtualPointsNumber++;
			e->connectedPoints.clear();
			innerBezierC0.Add(e);

			e->transpose->SetObjectPosition(
				(g->transpose->GetPosition() + posNextF) * 0.5f);

			e->connectedPoints.push_back(At(i + 1));
		}
	}
	innerBezierC0.Update();
}

void BezierC2::ChangeShowBezierCurve()
{
	innerBezierC0.ChangeShowBezierCurve();
}

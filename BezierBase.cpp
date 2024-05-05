#include "BezierBase.h"
#include "OpenGLHelper.h"

BezierBase::BezierBase( const char* name) : BezierBase( name, "##BezierBase", FigureType::CenterPoint)
{}

BezierBase::BezierBase() : BezierBase("BezierBase")
{}

BezierBase::BezierBase(const char* name, const char* uniqueName, FigureType type) : CenterPoint(name, uniqueName, type)
{
	CreateBezier();
	SetColor(glm::vec4(1, 0, 0, 1));
}

void BezierBase::Draw(GLFWwindow* window, const Camera& camera) {

	if (IsSomethingChange())
		Update();
}

void BezierBase::ActiveImGui() {
	ImGui::BeginGroup();
	{
		Figure::ActiveImGui();
		RotationInterfers();
		ScaleInterfers();

		if (ImGui::RadioButton("Show Bezier's Curve", showBezierC0))
			ChangeShowBezierC0();
		ImGui::SameLine();
		if (ImGui::RadioButton("Show Bezier's polynomial", showBezierPol))
			ChangeShowBezierPol();

		SelectedPoints();
	}
	ImGui::EndGroup();
}

bool BezierBase::Inputs(GLFWwindow* window, const Camera& camera) {
	return false;
}

bool BezierBase::IsValid(Figure* figure) {
	return figure->GetType() == FigureType::Point;
}

void BezierBase::Update() {
	CenterPoint::Update();
	CreateBezier();
	somethingHasChange = false;
}

bool BezierBase::AddContainer(FigureContainer* fc)
{
	this->UnMark();
	fc->Erase(this);
	return fc->Add(*(FigureContainer*)this);
}

void BezierBase::MarkFigure(Figure* f)
{
}

void BezierBase::CreateBezier() {
}

void BezierBase::SelectedPoints()
{
	int i = 0;
	for (std::set<Figure* >::iterator iter = selectedFigures.begin();
		iter != selectedFigures.end(); iter++)
	{
		auto figure = (*iter);

		char buf[100];
		sprintf_s(buf, "%d. %s", i, figure->name);

		if (ImGui::Selectable(buf)) {
			Erase(figure);
			break;
		}
		i++;
	}
}

void BezierBase::ChangeShowBezierC0()
{
	showBezierC0 = !showBezierC0;
}

void BezierBase::ChangeShowBezierPol()
{
	showBezierPol = !showBezierPol;
}

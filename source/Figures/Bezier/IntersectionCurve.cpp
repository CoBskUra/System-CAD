#include "IntersectionCurve.h"
#include "Figures/Intersection/IntersectionAble.h"

void IntersectionCurve::ConvertToInterpolated()
{
	for (int i = 0; i < points.size(); i++) {
		scene->AddFigure(points[i]);
	}
	scene->AddFigure(curve);
	scene->DeleteFigureById(GetId());
}

glm::vec3 IntersectionCurve::Derivative(float t)
{
	return curve->Derivative(t);
}

glm::vec3 IntersectionCurve::PositionOnCurve(float t)
{
	return curve->PositionOnCurve(t);
}

IntersectionCurve::IntersectionCurve(std::vector<glm::vec2> parametryzationVector, std::shared_ptr<Figure> figure, Scene* scene):BezierCurve("IntersectionCurve", FigureType::IntersectionCurve)
{
	auto intersection = dynamic_cast<IntersectionAble*>(figure.get());

	intersectedFigure = figure;
	curve = std::make_shared<BezierInterpolated>("BezierInterpolated-Intersection");
	this->scene = scene;

	if(curve->showBezierPol)
		curve->ChangeShowBezierPol();

	points.reserve(parametryzationVector.size());
	params = parametryzationVector;
	for (glm::vec2 pos : parametryzationVector) {
		std::shared_ptr<Point> point = std::make_shared<Point>();
		point->transpose->SetObjectPosition(intersection->Parametrization(pos.x, pos.y));
		curve->Add(point.get());
		points.push_back(point);
	}
}

void IntersectionCurve::Draw(GLFWwindow* window, const Camera& camera)
{
	if (intersectedFigure.expired()) {
		scene->DeleteFigureById(GetId());
		return;
	}
	curve->Draw(window, camera);
}

void IntersectionCurve::FigureSpecificImGui()
{
	if (ImGui::Button("Convert to Interpolated##IntersectionCurve")) {
		ConvertToInterpolated();
	}
}



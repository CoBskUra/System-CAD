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

	if(!curve->showBezierPol)
		curve->ChangeShowBezierPol();
	if (curve->showBezierCurve)
		curve->ChangeShowBezierCurve();
	

	points.reserve(parametryzationVector.size());
	params = parametryzationVector;
	for (glm::vec2 pos : parametryzationVector) {
		std::shared_ptr<Point> point = std::make_shared<Point>();
		point->transpose->SetObjectPosition(intersection->Parametrization(pos.x, pos.y));
		curve->Add(point.get());
		points.push_back(point);
	}

	curve->Unmark();
	SetUnmarkColor({ 0.215, 1, 0.129f, 1 });
	
	auto intersectionAble = dynamic_cast<IntersectionAble*>(figure.get());
	glm::vec2 field_v = intersectionAble->Field_v();
	glm::vec2 field_u = intersectionAble->Field_u();
	float length_v = field_v.y - field_v.x;
	float length_u = field_u.y - field_u.x;
	std::vector<float> data(N * N );
	for (glm::vec2 pos : parametryzationVector)
	{
		int i = (N - 1) * (pos.x - field_v.x) / length_v;
		int j = (N - 1) * (pos.y - field_u.x) / length_u;
		data[i * N + j] = 1;
	}

	texture.Recreat();
	texture.Bind();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, N, N, 0, GL_RED, GL_FLOAT, data.data());
	texture.Unbind();
}

void IntersectionCurve::Draw(GLFWwindow* window, const Camera& camera)
{
	if (intersectedFigure.expired()) {
		scene->DeleteFigureById(GetId());
		return;
	}

	curve->Draw(window, camera);
}

void IntersectionCurve::ActiveImGui()
{
	Figure::ActiveImGui();
}

void IntersectionCurve::FigureSpecificImGui()
{
	if (ImGui::Button("Convert to Interpolated##IntersectionCurve")) {
		ConvertToInterpolated();
	}
	ImGui::Text("pointer = %x", texture.ID);
	ImGui::Text("size = %d x %d", N, N);
	ImGui::Image((void*)(intptr_t)texture.ID, ImVec2(N, N));
}

glm::vec4 IntersectionCurve::SetUnmarkColor(glm::vec4 color)
{
	curve->SetUnmarkColor(color);
	return Figure::SetUnmarkColor(color);
}

void IntersectionCurve::SetMarkColor(glm::vec4 color)
{
	curve->SetUnmarkColor(color);
	Figure::SetMarkColor(color);
}

void IntersectionCurve::SetShowColor(glm::vec4 color)
{
	curve->SetShowColor(color);
	Figure::SetShowColor(color);
}



#pragma once
#include "BezierCurve.h"
#include "BezierInterpolated.h"

class IntersectionCurve : public BezierCurve {

	std::weak_ptr<Figure> intersectedFigure;
	std::shared_ptr<BezierInterpolated> curve;
	std::vector<glm::vec2> params;
	std::vector<std::shared_ptr<Point>> points;
	Scene* scene;
	void ConvertToInterpolated();
public:

	IntersectionCurve(std::vector<glm::vec2> params, std::shared_ptr<Figure> figure, Scene* scene);
	glm::vec3 Derivative(float t) override;
	glm::vec3 PositionOnCurve(float t) override;
	void Draw(GLFWwindow* window, const Camera& camera) override;
	void FigureSpecificImGui() override;
};
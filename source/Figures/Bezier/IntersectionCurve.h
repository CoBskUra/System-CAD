#pragma once
#include "BezierCurve.h"
#include "BezierInterpolated.h"
#include "ShaderManadement/Texture.h"
#include "Figures/Intersection/IntersectionAble.h"

class IntersectionCurve : public BezierCurve {

	Texture texture{ GL_TEXTURE_2D };
	const int N = 256;
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

	void ActiveImGui() override;
	void FigureSpecificImGui() override;


	glm::vec4 SetUnmarkColor(glm::vec4 color) override;
	void SetMarkColor(glm::vec4 color) override;
	void SetShowColor(glm::vec4 color) override;
};
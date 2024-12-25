#pragma once
#include "BezierCurve.h"
#include "BezierInterpolated.h"
#include "ShaderManadement/Texture.h"
#include "Figures/Intersection/IntersectionAble.h"
#include <queue>

class IntersectionCurve : public BezierCurve, public IntersectionResultInfo {
	Texture texture{ GL_TEXTURE_2D };
	const int N = IntersectionAble::N;
	std::weak_ptr<Figure> intersectedFigure;
	std::shared_ptr<BezierInterpolated> curve;
	std::vector<glm::vec2> params;
	std::vector<float> data;
	std::vector<std::shared_ptr<Point>> points;
	Scene* scene;
	bool includeIntersection = false;
	bool showRed = false;
	void ConvertToInterpolated();
public:

	IntersectionCurve(std::vector<glm::vec2> params, std::shared_ptr<Figure> figure, Scene* scene);
	IntersectionCurve(std::vector<glm::vec2> params, std::shared_ptr<Figure> figure, IntersectionAble* intersectionAble, Scene* scene);

	int NumberOfPoints() override;
	float MaxValue() override;
	glm::vec3 Derivative_2(float t) override;
	glm::vec3 Derivative(float t) override;
	glm::vec3 PositionOnCurve(float t) override;
	void Draw(GLFWwindow* window, const Camera& camera) override;

	void ActiveImGui() override;
	void FigureSpecificImGui() override;


	glm::vec4 SetUnmarkColor(glm::vec4 color) override;
	void SetMarkColor(glm::vec4 color) override;
	void SetShowColor(glm::vec4 color) override;
	void BresenhamLineWraped(std::vector<float>& data, const int x1, const int y1, const int x2, const int y2);
	void FloodFill(std::vector<float>& data, glm::bvec2 wrap);

	const std::vector<float>& GetResultData() override;
	GLuint TextureId() override;

	void Reverse() override;

	~IntersectionCurve();
};
#pragma once
#include "Figures/Bezier/BezierBase.h"
#include "SceneManadement/Scene.h"

class BezierSurface : public BezierBase {
public:
	enum class CreationType {
		surface,
		cylinder,
		COUNT // Dodajemy COUNT na koñcu
	};

	inline const char* ToString(CreationType a) {
		switch (a)
		{
		case BezierSurface::CreationType::surface:
			return "surface";
			break;
		case BezierSurface::CreationType::cylinder:
			return "cylinder";
			break;
		default:
			break;
		}
	}
protected:


	glm::ivec2 windowSize;

	VAO vao;
	const Shader& shader;

	VAO vao_curve;
	const Shader& shader_curve;

	glm::vec4 curveColor{ 1, 0, 1, 1 };
	int numberOfIndes{};

	std::vector<std::shared_ptr<Figure>> controlPoints;
	int numberOfVertexes{};
	int patch_div = 4;

	CreationType creationType = CreationType::surface;
	int verticalNum = 1, horizontalNum = 1;
	float radius = 1, height = 1;
	bool accepted = false, openWindow = true, firstTime = true;

	virtual int MaxSize();
	void ResizeControlPoints();
	virtual bool CreationWindowInterfers(glm::ivec2 appWindowSize);
	void CreateBezierVAO() override;
	void DeleteRangeControlPoints(int start, int end);
	virtual glm::vec3 GeneratePosForVertexInPatch(int verticalID, int horizontalID, int k1, int k2);
	bool Swap(Figure* from, std::shared_ptr<Figure> to) override;
public:
	Scene* refrenceScene;

	BezierSurface(const char* name, FigureType type, const Shader& shader_surface, const Shader& shaderCurve);
	bool Inputs(GLFWwindow* window, const Camera& camera) override;
	void Draw(GLFWwindow* window, const Camera& camera)override;
	void ActiveImGui()override;
	void TurnOffStartupInterfers();
	CreationType GetWrapType();
	virtual glm::vec3 DerivativeV(int patchV, int pathH, float v, float u) = 0;
	virtual glm::vec3 DerivativeU(int patchV, int pathH, float v, float u) = 0;
	virtual glm::vec3 DerivativeVU(int patchV, int pathH, float v, float u) = 0;

	glm::ivec2 SurfaceSize();
	virtual Figure* TakePoint(int verticalID, int horizontalID, int k1, int k2) const = 0;
	virtual int TakeId(int verticalID, int horizontalID, int k1, int k2) const = 0;
};
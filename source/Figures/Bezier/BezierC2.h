#pragma once
#include "BezierBase.h"
#include "BezierC0.h"
#include "Figures/CanterPoint.h"
#include "Figures/VirtualPoint.h"
#include "ShaderRefrence/StaticShaders.h"
#include "Models/Curves/BezierC2.h"
#include "SceneManadement/Scene.h"

class BezierC2: public BezierBase
{
	BezierC0 innerBezierC0;
	std::vector<VirtualPoint*> virtualPoints;
	int virtualPointsNumber = 0;
	glm::vec4 virtualPointColor = { 1,0,1,1 };


public:
	bool showVirtualPointsInterfers = false;
	bool showVirtualPoints = true;

	BezierC2(const char* name);
	BezierC2();
	BezierC2(MG1::BezierC2 b0, const Scene* scene, int idOffset = 0);
	MG1::BezierC2 Serialize(int idOffset) const;

	virtual ~BezierC2();

	void Draw(GLFWwindow* window, const Camera& camera)  override;
	void ActiveImGui() override;
	bool Inputs(GLFWwindow* window, const Camera& camera) override;

	void CreateBezierVAO() override;
	void ChangeShowBezierCurve() override;
	void ChangeShowBezierPol() override;

protected:
	BezierC2( const char* name,  FigureType type);
};


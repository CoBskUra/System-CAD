#pragma once
#include "BezierBase.h"
#include "BezierC2.h"
#include "Models/Curves/InterpolatedC2.h"
#include "SceneManadement/Scene.h"

class BezierInterpolated: public BezierBase
{
	std::vector<VirtualPoint*> virtualPoints;
	int virtualPointsNumber = 0;
	BezierC0 innerBezierC0;
public:
	BezierInterpolated( const char* name);
	BezierInterpolated();
	BezierInterpolated(MG1::InterpolatedC2 b0, const Scene* scene, int idOffset = 0);
	MG1::InterpolatedC2 Serialize(int idOffset) const;

	void virtual Draw(GLFWwindow* window, const Camera& camera);

	void virtual CreateBezier();
	void virtual ChangeShowBezierCurve();

protected:
	BezierInterpolated(const char* name, FigureType type);
	void virtual ChangeShowBezierPol();
	float TakePointDelta(int i);
	float TakePointDelta(std::vector<glm::vec3> points, int i);
	void AddPoint(BezierC0& bezier, glm::vec3 pos);
};


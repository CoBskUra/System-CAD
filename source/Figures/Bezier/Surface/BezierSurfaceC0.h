#pragma once
#include "Figures/VirtualPoint.h"
#include "SceneManadement/Scene.h"
#include <Models/Surfaces/BezierSurfaceC0.h>
#include "BezierSurface.h";



class BezierSurfaceC0: public BezierSurface
{
public:

	BezierSurfaceC0(const char* name);
	BezierSurfaceC0(MG1::BezierSurfaceC0 bs2, Scene* scene, int idOffset);
	BezierSurfaceC0();
	~BezierSurfaceC0();
	MG1::BezierSurfaceC0 Serialize(int idOffset) const;
	Figure* TakePoint(int verticalID, int horizontalID, int k1, int k2) const override;
	int TakeId(int verticalID, int horizontalID, int k1, int k2) const override;

	glm::vec3 DerivativeV(int patchV, int pathH, float v, float u)	override;
	glm::vec3 DerivativeU(int patchV, int pathH, float v, float u)	override;
	glm::vec3 DerivativeVU(int patchV, int pathH, float v, float u)	override;

	glm::vec4 Bernstain3D(float t);
	glm::vec4 Bernstain3DDerivative(float t);
	glm::vec3 Bernstain2D(float t);
	glm::vec3 BernstainPolinomalDe(float t);
protected:
	BezierSurfaceC0(const char* name, FigureType type);
private:
	std::vector<glm::mat<4, 4, float>> ControlPointsMatrix(int patchV, int pathH);
	
	int MaxSize() override;;
};


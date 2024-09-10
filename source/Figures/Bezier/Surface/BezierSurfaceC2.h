#pragma once
#include "BezierSurface.h"
#include "Figures/VirtualPoint.h"
#include "SceneManadement/Scene.h"
#include <imgui_impl_opengl3.h>
#include <Models/Surfaces/BezierSurfaceC2.h>



class BezierSurfaceC2: public BezierSurface
{


public:

	BezierSurfaceC2(const char* name);
	BezierSurfaceC2(MG1::BezierSurfaceC2 bs2, Scene* scene, int idOffset);
	BezierSurfaceC2();
	~BezierSurfaceC2();

	MG1::BezierSurfaceC2 Serialize(int idOffset) const;
	glm::vec3 DerivativeV(int patchV, int pathH, float v, float u)  override { return{};};
	glm::vec3 DerivativeU(int patchV, int pathH, float v, float u)  override { return{};};
	glm::vec3 DerivativeVU(int patchV, int pathH, float v, float u) override { return{};};


protected:
	BezierSurfaceC2(const char* name, FigureType type);
	glm::vec3 GeneratePosForVertexInPatch(int verticalID, int horizontalID, int k1, int k2) override;

private:
	Figure* TakePoint(int verticalID, int horizontalID, int k1, int k2) const override;
	int TakeId(int verticalID, int horizontalID, int k1, int k2) const override;
	int MaxSize() override;
};


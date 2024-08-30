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

protected:
	BezierSurfaceC0(const char* name, FigureType type);
private:
	Figure* TakePoint(int verticalID, int horizontalID, int k1, int k2) const override;
	int TakeId(int verticalID, int horizontalID, int k1, int k2) const override;
	int MaxSize() override;;
};


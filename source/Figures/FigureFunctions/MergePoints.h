#pragma once
#include <SceneManadement/Scene.h>
#include <Figures/Point.h>

class MergePoints {
	Scene* scene;
public:
	MergePoints(Scene* scene);
	bool Merge(FigureContainer* figureContainer);
};
#pragma once
#include <vector>
#include <set>
#include "Figures/Figure.h"
#include "Container/FigureContainer.h";

class Scene {
public:
	std::vector<Figure*> figures;
	std::set<FigureContainer*> activeFigureContainers;
	std::set<FigureContainer*> figureContainers;

	std::vector< const char*> names;
	std::vector<bool> active;


	int NumberOfActive();
	Figure* at(int i) ;
	void AddFigure(Figure* figure);
	void DeleteFigure(int id);
	bool ChangeActiveState(int i);
	int Size();

	~Scene();

	void DeleteAll();
	void DeleteLastFigure();

private:
	int activeCount{ 0 };
};
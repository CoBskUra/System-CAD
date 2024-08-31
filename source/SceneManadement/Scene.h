#pragma once
#include <vector>
#include <map>
#include <set>
#include "Figures/Figure.h"
#include "Container/FigureContainer.h";

class Scene {
public:
	std::map<int, std::shared_ptr<Figure>> figures;
	std::vector<int> insertionOrder;

	std::set<FigureContainer*> activeFigureContainers;
	std::set<FigureContainer*> figureContainers;

	std::vector<bool> active;


	int NumberOfActive();
	std::shared_ptr<Figure> at(int i) const;
	std::shared_ptr<Figure> byID(uint32_t id) const;
	void AddFigure(std::shared_ptr<Figure> figure);
	void DeleteFigureAt(int place);
	void DeleteFigureById(uint32_t id);
	bool ChangeActiveState(int i);
	int Size() const;
	int SmalestIdOnScene() const;
	bool IsDeleteAble(int place);

	~Scene();

	void DeleteAll();
	void DeleteLastFigure();

private:
	int activeCount{ 0 };
};
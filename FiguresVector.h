#pragma once
#include <vector>
#include <set>
#include "Figure.h"
#include "FigureContainer.h";

class FiguresVector {
public:
	std::vector<Figure*> figures;
	std::set<FigureContainer*> activeFigureContainers;
	std::set<FigureContainer*> figureContainers;

	std::vector< const char*> names;
	std::vector<bool> active;

	FigureContainer* testContainer;

	int NumberOfActive() {
		return activeCount;
	}

	Figure* at(int i) { return figures[i]; };

	void AddFigure(Figure* figure) {
		figures.push_back(figure);
		names.push_back(figure->name);

		if (dynamic_cast<FigureContainer*>(figure))
		{
			figureContainers.insert({ dynamic_cast<FigureContainer*>(figure) });
		}
		else {
			std::set<FigureContainer*>::iterator iter;
			for (iter = activeFigureContainers.begin(); iter != activeFigureContainers.end(); iter++) {
				(*iter)->Add(figure);
			}
		}
		active.push_back(false);
	}

	void DeleteFigure(int id) {
		if (active[id])
			ChangeActiveState(id);
		active.erase(std::next(active.begin(), id));

		auto toDelete = figures[id];
		figures.erase(std::next(figures.begin(), id));
		names.erase(std::next(names.begin(), id));
		if (toDelete->GetType() == FigureType::BezierSurfaceC0)
		{
			FigureContainer* fc = dynamic_cast<FigureContainer*>(toDelete);
			while (Size() > 0 && fc->Contain(at(id)))
				DeleteFigure(id);
		}

		if ( !toDelete->HaveOwner() )
		{
			delete toDelete;
			toDelete = NULL;
		}
	}

	bool ChangeActiveState(int i) {
		active[i] = !active[i];

		if (active[i]) {
			activeCount++;
			auto castedFigureToContainer = dynamic_cast<FigureContainer*>( figures.at(i));
			if (castedFigureToContainer)
				activeFigureContainers.insert(castedFigureToContainer);
		}
		else {
			activeCount--;
			auto castedFigureToContainer = dynamic_cast<FigureContainer*>(figures.at(i));
			if (castedFigureToContainer)
				activeFigureContainers.erase(castedFigureToContainer);
		}
		return active[i];
	}

	int Size() {
		return figures.size();
	}

	~FiguresVector() {
		Delete();
	}

	void Delete() {
		while (Size() > 0)
			DeleteFigure(0);
	}
private:
	int activeCount;
};
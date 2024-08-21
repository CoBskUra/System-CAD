#include "Scene.h"

int Scene::NumberOfActive() {
	return activeCount;
}

Figure* Scene::at(int i) { return figures[i]; }

void Scene::AddFigure(Figure* figure) {
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

void Scene::DeleteFigure(int id) {
	if (active[id])
		ChangeActiveState(id);
	active.erase(std::next(active.begin(), id));

	auto toDelete = figures[id];
	figures.erase(std::next(figures.begin(), id));
	names.erase(std::next(names.begin(), id));

	if (dynamic_cast<FigureContainer*>(toDelete))
	{
		FigureContainer* fc = dynamic_cast<FigureContainer*>(toDelete);
		activeFigureContainers.erase(fc);
		figureContainers.erase(fc);
	}

	if (toDelete->IsOwner())
	{
		FigureContainer* fc = dynamic_cast<FigureContainer*>(toDelete);
		while (Size() > id && at(id)->IsMyOwner(toDelete))
			if (at(id)->NumberOfContainers() == 1)
				DeleteFigure(id);
			else
				id++;
	}

	if (!toDelete->HaveOwner())
	{
		delete toDelete;
		toDelete = NULL;
	}


}

bool Scene::ChangeActiveState(int i) {
	active[i] = !active[i];

	if (active[i]) {
		activeCount++;
		auto castedFigureToContainer = dynamic_cast<FigureContainer*>(figures.at(i));
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

int Scene::Size() {
	return figures.size();
}

Scene::~Scene() {
	DeleteAll();
}

void Scene::DeleteAll() {
	while (Size() > 0)
		DeleteFigure(0);
}

void Scene::DeleteLastFigure() {
	DeleteFigure(Size() - 1);
}
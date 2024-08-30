#include "Scene.h"

int Scene::NumberOfActive() {
	return activeCount;
}

std::shared_ptr<Figure> Scene::at(int i) const { return  figures.at(insertionOrder[i]); }

std::shared_ptr<Figure> Scene::byID(uint32_t id) const
{
	if (figures.contains(id))
		return figures.at(id);
	else
		return nullptr;
}

void Scene::AddFigure(std::shared_ptr<Figure> figure) {
	if(figures.contains(figure->GetId()))
		return;

	figures[figure->GetId()] =  figure;
	insertionOrder.push_back(figure->GetId());

	if (dynamic_cast<FigureContainer*>(figure.get()))
	{
		figureContainers.insert({ dynamic_cast<FigureContainer*>(figure.get()) });
	}
	else {
		std::set<FigureContainer*>::iterator iter;
		for (iter = activeFigureContainers.begin(); iter != activeFigureContainers.end(); iter++) {
			(*iter)->Add(figure.get());
		}
	}
	active.push_back(false);
}

void Scene::DeleteFigureAt(int place) {
	if (active[place])
		active[place] = false;

	auto toDelete = at( place);

	insertionOrder.erase(std::next(insertionOrder.begin(), place));
	figures.erase(toDelete->GetId());
	active.erase(std::next(active.begin(), place));

	if (dynamic_cast<FigureContainer*>(toDelete.get()))
	{
		FigureContainer* fc = dynamic_cast<FigureContainer*>(toDelete.get());
		activeFigureContainers.erase(fc);
		figureContainers.erase(fc);
	}

	if (toDelete->IsOwner())
	{
		FigureContainer* fc = dynamic_cast<FigureContainer*>(toDelete.get());
		int i = 0;
		while (Size() > i )
			if (at(i)->NumberOfContainers() == 1 && at(i)->IsMyOwner(toDelete.get()))
				DeleteFigureAt(i);
			else
				i++;
	}

	//if (!toDelete->HaveOwner())
	//{
	/*	toDelete.reset();
		toDelete = NULL;*/
	//}
}

void Scene::DeleteFigureById(uint32_t id)
{
	auto it = std::find(insertionOrder.begin(), insertionOrder.end(), id);
	if (it == insertionOrder.end())
		return;

	int place = it - insertionOrder.begin();
	DeleteFigureAt(place);
}

bool Scene::ChangeActiveState(int i) {
	active[i] = !active[i];

	if (active[i]) {
		activeCount++;
		auto castedFigureToContainer = dynamic_cast<FigureContainer*>(at(i).get());
		if (castedFigureToContainer)
			activeFigureContainers.insert(castedFigureToContainer);
	}
	else {
		activeCount--;
		auto castedFigureToContainer = dynamic_cast<FigureContainer*>(at(i).get());
		if (castedFigureToContainer)
			activeFigureContainers.erase(castedFigureToContainer);
	}
	return active[i];
}

int Scene::Size() const {
	return figures.size();
}

int Scene::SmalestIdOnScene() const
{
	int smalest = std::numeric_limits<int>::max();
	for (auto f : figures) {
		if (f.second->GetId() < smalest)
			smalest = f.second->GetId();
	}
	return smalest;
}

Scene::~Scene() {
	//DeleteAll();
}

void Scene::DeleteAll() {
	
	while (Size() > 0)
		DeleteFigureAt(0);
}

void Scene::DeleteLastFigure() {
	DeleteFigureAt(Size() - 1);
}
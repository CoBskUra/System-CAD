#include "FigureContainer.h"

bool FigureContainer::IsValid(Figure* figure)
{
	return true;
}

bool FigureContainer::Add(Figure* figure) {
	if (!IsValid(figure))
		return false;

	auto pair = selectedFigures.insert({ figure->GetUniqueName() , figure });
	figure->Mark();
	if (pair.second)
		Update();
	return pair.second;
}

bool FigureContainer:: Erase(Figure* figure) {
	bool erased = selectedFigures.erase(figure->GetUniqueName());
	figure->UnMark();
	if (erased)
		Update();
	return erased;
}

bool FigureContainer:: Contain(Figure* figure) {
	return selectedFigures.find(figure->GetUniqueName()) != selectedFigures.end();
}

int FigureContainer::ContainerSize()
{
	return selectedFigures.size();
}

void FigureContainer::Update()
{
}

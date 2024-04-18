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
	if (pair.second) {
		figure->AddContainer(this);
		SomethingHasChange();
		valueAdded = true;
	}
	return pair.second;
}

bool FigureContainer::Add(const FigureContainer& figureCoatiner)
{
	bool addAny = false;
	std::map<std::string, Figure* >::const_iterator iter;
	for (iter = figureCoatiner.selectedFigures.begin(); iter != figureCoatiner.selectedFigures.end(); iter++)
	{
		if( Add((*iter).second)) {
			addAny = true;
		}
	}
	return addAny;
}

bool FigureContainer:: Erase(Figure* figure) {
	bool erased = selectedFigures.erase(figure->GetUniqueName());
	figure->UnMark();
	if (erased) {
		figure->EraseContainer(this);
		SomethingHasChange();
		valueErased = true;
	}
	return erased;
}

bool FigureContainer:: Contain(Figure* figure) {
	return selectedFigures.find(figure->GetUniqueName()) != selectedFigures.end();
}

int FigureContainer::ContainerSize()
{
	return selectedFigures.size();
}

void FigureContainer::SomethingHasChange()
{
	if(!somethingHasChange)
		somethingHasChange = true;
}

bool FigureContainer::IsSomethingChange()
{
	return somethingHasChange;
}

void FigureContainer::Update()
{
}

FigureContainer::~FigureContainer()
{
	while (selectedFigures.begin() != selectedFigures.end())
	{
		Erase((*selectedFigures.begin()).second);
	}
}

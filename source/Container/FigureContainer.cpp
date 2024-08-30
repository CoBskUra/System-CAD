#include "FigureContainer.h"

bool FigureContainer::IsValid(Figure* figure)
{
	return true;
}

bool FigureContainer::Add(Figure* figure) {
	if (!IsValid(figure))
		return false;

	auto pair = selectedFigures.insert(figure);
	MarkFigure(figure);
	if (pair.second) {
		orderdFigures.push_back(figure);
		figure->AddContainer(this);
		SomethingHasChange();
		valueAdded = true;
	}
	return pair.second;
}

bool FigureContainer::Add(FigureContainer* figureCoatiner) {
	return Add(*figureCoatiner);
}

bool FigureContainer::Add(const FigureContainer& figureCoatiner)
{
	bool addAny = false;
	for (auto iter = figureCoatiner.orderdFigures.begin(); iter != figureCoatiner.orderdFigures.end(); iter++)
	{
		if( Add(*iter)) {
			addAny = true;
		}
	}
	return addAny;
}

bool FigureContainer:: Erase(Figure* figure) {
	bool erased = selectedFigures.erase(figure);
	figure->UnMark();
	if (erased) {
		for (auto iter = orderdFigures.begin(); iter != orderdFigures.end(); iter++) {
			if (figure == *iter) {
				orderdFigures.erase(iter);
				break;
			}
		}
		figure->EraseContainer(this);
		SomethingHasChange();
		valueErased = true;
	}
	return erased;
}

bool FigureContainer:: Contain(Figure* figure) {
	return selectedFigures.find(figure) != selectedFigures.end();
}

void FigureContainer::Clear()
{
	while (selectedFigures.begin() != selectedFigures.end())
	{
		auto figure = *selectedFigures.begin();
		selectedFigures.erase(selectedFigures.begin());
		figure->UnMark();
		figure->EraseContainer(this);
	}
	SomethingHasChange();
	valueErased = true;

	selectedFigures.clear();
	orderdFigures.clear();
}

int FigureContainer::ContainerSize() const
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

void FigureContainer::MarkFigure(Figure* figure)
{
}

void FigureContainer::Update()
{

}

bool FigureContainer::Swap(Figure* from, Figure* to)
{
	if (from->GetType() != to->GetType())
		return false;

	if (!Contain(from))
		return false;

	from->UnMark();
	to->Mark();
	if (!Contain(to))
	{
		for (auto iter = orderdFigures.begin(); iter != orderdFigures.end(); iter++) {
			if (from == *iter) {
				(*iter) = to;
				break;
			}
		}
	}
	else
	{
		Erase(from);
	}

	selectedFigures.erase(from);
	selectedFigures.insert(to);

	from->EraseContainer(this);
	SomethingHasChange();
	valueErased = true;
	valueAdded = true;
	return true;
}

Figure* FigureContainer::At(int i) const
{
	return orderdFigures.at(i);
}

FigureContainer::~FigureContainer()
{
	Clear();
}

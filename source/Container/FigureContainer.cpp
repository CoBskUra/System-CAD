#include "FigureContainer.h"

bool FigureContainer::IsValid(Figure* figure)
{
	auto figureContainer = dynamic_cast<FigureContainer*>(figure);
	if (figureContainer != nullptr && figureContainer == this)
		return false;
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
	UnmarkFigure(figure);
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
		UnmarkFigure(figure);
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

void FigureContainer::UnmarkFigure(Figure* figure)
{
	figure->Unmark();
}

void FigureContainer::Update()
{
	valueErased = valueAdded = somethingHasChange = false;
}

bool FigureContainer::Swap(Figure* from, std::shared_ptr<Figure> to)
{
	if (from->GetType() != to->GetType())
		return false;

	if (!Contain(from))
		return false;

	from->Unmark();
	to->Mark();
	if (!Contain(to.get()))
	{
		for (auto iter = orderdFigures.begin(); iter != orderdFigures.end(); iter++) {
			if (from == *iter) {
				(*iter) = to.get();
				break;
			}
		}
	}
	else
	{
		Erase(from);
	}

	selectedFigures.erase(from);
	selectedFigures.insert(to.get());

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

#pragma once
#include "Figure.h"
#include <set>
class Figure;

class FigureContainer
{
protected:
	std::vector<Figure*> orderdFigures;
	std::set<Figure* > selectedFigures;
	bool somethingHasChange;
	bool valueAdded;
	bool valueErased;

	bool virtual IsValid(Figure* figure);
public:
	bool Add(Figure* figure);
	bool Add(const FigureContainer& figure);
	bool Erase(Figure* figure);
	bool Contain(Figure* figure);
	int ContainerSize();
	void SomethingHasChange();
	bool IsSomethingChange();
	void virtual MarkFigure(Figure* figure);
	void virtual Update();

	virtual ~FigureContainer();
};


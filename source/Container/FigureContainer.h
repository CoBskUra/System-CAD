#pragma once
#include "Figures/Figure.h"
#include <set>
class Figure;

class FigureContainer
{// todo rewrite to use smartPointers
protected:
	std::vector<Figure*> orderdFigures;
	std::set<Figure*> selectedFigures;
	bool somethingHasChange;
	bool valueAdded;
	bool valueErased;

	bool virtual IsValid(Figure* figure);
public:
	bool Add(Figure* figure);
	bool Add(FigureContainer* figureCoatiner);
	bool Add(const FigureContainer& figure);

	bool Erase(Figure* figure);
	bool Contain(Figure* figure);
	void Clear();
	int ContainerSize() const;
	void SomethingHasChange();
	bool IsSomethingChange();
	void virtual MarkFigure(Figure* figure);
	void virtual UnmarkFigure(Figure* figure);
	void virtual Update();
	bool virtual Swap(Figure* from, std::shared_ptr<Figure> to);
	Figure* At(int i) const;

	virtual ~FigureContainer();
};


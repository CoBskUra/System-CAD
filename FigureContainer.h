#pragma once
#include "Figure.h"
#include <map>
class FigureContainer
{
protected:
	std::map<std::string, Figure* > selectedFigures;
	bool virtual IsValid(Figure* figure);
public:
	bool Add(Figure* figure);
	bool Erase(Figure* figure);
	bool Contain(Figure* figure);
	int ContainerSize();

	void virtual Update();
};


#include "MergePoints.h"

MergePoints::MergePoints(Scene* scene)
{
	this->scene = scene;
}

bool MergePoints::Merge(FigureContainer* figureContainer)
{
	if (figureContainer->ContainerSize() <= 1)
		return false;
	std::vector<Figure*> savePoints;
	for (int i = 0; i < figureContainer->ContainerSize(); i++) {
		savePoints.push_back(figureContainer->At(i));
		if (FigureType::Point != figureContainer->At(i)->GetType())
			return false;
	}
	
	std::shared_ptr<Point> avgPoint = std::make_shared<Point>( "Avg Point" );
	glm::vec3 avgPos{ 0,0,0 };

	int i = 0;
	for (i = 0; i < savePoints.size(); i++) {
		avgPos += savePoints.at(i)->transpose->GetPosition();
		if (!savePoints.at(i)->Swap(avgPoint.get()))
			throw "Something go wrong during swap";
		scene->DeleteFigureById(savePoints.at(i)->GetId());
	}
	
	avgPos = avgPos / (float)i;
	avgPoint->transpose->SetObjectPosition(avgPos);
	scene->AddFigure(avgPoint);
	figureContainer->SomethingHasChange();

	return true;
}
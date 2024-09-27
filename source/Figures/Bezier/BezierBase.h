#pragma once
#include "Figures/CanterPoint.h"
#include "Container/FigureContainer.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <vector>
#include "Models/Curves/Bezier.h"

class BezierBase : public CenterPoint {
public:
	bool showBezierCurve = true;
	bool showBezierPol = true;

	BezierBase( const char* name);
	BezierBase();
	MG1::Bezier Serialize(int idOffset) const
	{
		MG1::Bezier bezier{};
		bezier.SetId(GetId());
		bezier.name = name;
		for (int i = 0; i < ContainerSize(); i++)
		{
			MG1::PointRef p{ At(i)->GetId() - idOffset };

			bezier.controlPoints.push_back(p);
		}
		return bezier;
	}

	void Draw(GLFWwindow* window, const Camera& camera) override;
	void ActiveImGui() override;
	bool Inputs(GLFWwindow* window, const Camera& camera) override;

	bool IsValid(Figure* figure) override;
	void Update() override;
	bool AddContainer(FigureContainer* fc) override;
	void MarkFigure(Figure* f) override;

	void virtual ChangeShowBezierCurve();
	void virtual ChangeShowBezierPol();
	glm::ivec3 Size();
protected:
	BezierBase(const char* name, FigureType type);
	void virtual CreateBezierVAO();
	void SelectedPoints();
};
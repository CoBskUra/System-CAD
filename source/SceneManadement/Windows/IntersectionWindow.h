#pragma once

#pragma once
#include "Figures/Figure.h"
#include "WindowParent.h"
#include "Figures/Intersection/Intersection.h"
#include "Figures/Point.h"
#include "Figures/Bezier/BezierInterpolated.h"
#include "Figures/Bezier/IntersectionCurve.h"

class IntersectionWindow: public WindowParent {
public:
	IntersectionWindow() {
		name = "Intersection of figures";
	}

	std::weak_ptr<Figure> sceneObject_1;
	std::weak_ptr<Figure> sceneObject_2;

	GLFWwindow* window;
	Camera* camera;
	Cursor* cursor;
	bool useCursor = false;;
	float step = 0.005;
	float epsilon = 0.000001;


	std::weak_ptr<Figure>& Iterator(int i) {
		switch (i)
		{
		case 0: return sceneObject_1;
		case 1: return sceneObject_2;
		default:
			break;
		}
		throw std::out_of_range("Out of range");
		return sceneObject_2;
	}

	void Interferes(Scene* scene) override {

		int counter = 0;
		for(int i = 0; i < 2; i++)
		{
			std::shared_ptr<Figure> lockSceneObject;
			lockSceneObject = Iterator(i).lock();

			if (i == 0)
			{
				ImGui::BeginChild("Surfaces##TransformSurfaceWindow", 
					ImVec2(
						ImGui::GetContentRegionAvail().x * 0.5f,
						ImGui::GetContentRegionAvail().y * 0.8f), ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar);
			}
			else
				ImGui::BeginChild("Curve##TransformSurfaceWindow",
					ImVec2(
						0,
						ImGui::GetContentRegionAvail().y * 0.8f
					), ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar);

			for (int j = 0; j< scene->Size(); j++) {
				if (dynamic_cast<IntersectionAble*>(scene->at(j).get()) ) {
					auto figure = scene->at(j);

					char buf[200];
					sprintf_s(buf, "%d. %s", counter, figure->name);
					if (ImGui::RadioButton(buf, (!Iterator(i).expired()) &&
						figure->GetId() == lockSceneObject->GetId())) {
						Iterator(i) = figure;
						lockSceneObject = Iterator(i).lock();
					}
					counter++;
				}
			}
			ImGui::EndChild();
			if(i == 0)
				ImGui::SameLine();
			counter = 0;
		}
		if (ImGui::RadioButton("Use cursor##IntersectionWindow", useCursor)) {
			useCursor = !useCursor;
		}

		ImGui::DragFloat("Step##IntersectionWindow", &step, 0.005, 0.00001, 2, "%.7f");
		ImGui::DragFloat("Epsilon##IntersectionWindow", &epsilon, 0.00001, 0.000001, 1, "%.8f");

		if (ImGui::Button("Intersections##IntersectionWindow") &&
			!sceneObject_2.expired() && !sceneObject_1.expired()) {
			Intersection intersections;
			std::shared_ptr<Figure> figure_1 = sceneObject_1.lock();
			std::shared_ptr<Figure> figure_2 = sceneObject_2.lock();
			auto object_a = dynamic_cast<IntersectionAble*>(figure_1.get());
			auto object_b = dynamic_cast<IntersectionAble*>(figure_2.get());


			glm::vec4 closeParams{ 0, 0, 0, 0 };
			if (useCursor) {
				closeParams = intersections.RandomTheClosetToPoint(cursor->transpose->GetPosition(), object_a, object_b);
				auto pos = object_a->Parametrization(closeParams.x, closeParams.y);
				std::cout << std::endl << pos.x << " " << pos.y << " " << pos.z << std::endl;
				pos = object_b->Parametrization(closeParams.z, closeParams.w);
				std::cout << std::endl << pos.x << " " << pos.y << " " << pos.z << std::endl;
			}
			else if (figure_2->GetId() == figure_1->GetId())
				closeParams = intersections.TheFaresParams(object_a);
			else
				closeParams = intersections.RandomTheClosetToEachOther(object_a, object_b);

			

			auto params = intersections.FirstIntersectionPoint(object_a, object_b, closeParams);
			if (figure_2->GetId() == figure_1->GetId() &&
				abs(params.x - params.z) < 0.000001 &&
				abs(params.y - params.w) < 0.000001) {
				std::cout << "false" << std::endl;
				return;
			}
			std::cout<< (intersections.IsIntersected(object_a, object_b, params) ? "true" : "false" )<< std::endl;

			if (!intersections.IsIntersected(object_a, object_b, params))
				return;
			auto positions = intersections.IntersectionFrame(params, object_a, object_b, step, epsilon);

			auto curve_a = std::make_shared< IntersectionCurve>(positions.first, figure_1, scene);
			auto curve_b = std::make_shared< IntersectionCurve>(positions.second, figure_2, scene);
			scene->AddFigure(curve_a); 
			scene->AddFigure(curve_b);
		}
	}
};
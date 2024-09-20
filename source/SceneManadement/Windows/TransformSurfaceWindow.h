#pragma once
#include "Figures/Bezier/BezierCurve.h"
#include "Figures/Bezier/Surface/BezierSurface.h"
#include "Figures/FigureFunctions/TransformSurfercUsingCurve.h"
#include "WindowParent.h"

class TransformSurfaceWindow : public WindowParent {
public:

	float r = 0;
	std::weak_ptr<BezierSurface> sceneObject_1;
	std::weak_ptr<BezierCurve> sceneObject_2;

	TransformSurfaceWindow() {
		name = "Transform Surface with curve";
	}

	void Interferes(Scene* scene) override {

		int counterSur = 0;
		int counterCur = 0;
		std::shared_ptr<BezierSurface> lockSurface;
		lockSurface = sceneObject_1.lock();

		ImGui::BeginChild("Surfaces##TransformSurfaceWindow", ImVec2(ImGui::GetContentRegionAvail().x * 0.5f, 150), ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar);
		for (int i = 0; i < scene->Size(); i++) {
			if (dynamic_cast<BezierSurface*>( scene->at(i).get())) {
				auto sur = std::static_pointer_cast<BezierSurface>(scene->at(i));

				char buf[200];
				sprintf_s(buf, "%d. %s", counterSur, sur->name);
				if (ImGui::RadioButton(buf, !sceneObject_1.expired() && sur->GetId() == lockSurface->GetId())) {
					sceneObject_1 = sur;
				}
				counterSur++;
			}
			
		}
		ImGui::EndChild();
		ImGui::SameLine();


		std::shared_ptr<BezierCurve> lockCurve;
		lockCurve = sceneObject_2.lock();

		ImGui::BeginChild("Curve##TransformSurfaceWindow", ImVec2(0, 150), ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar);
		for (int i = 0; i < scene->Size(); i++) {
			if (dynamic_cast<BezierCurve*>(scene->at(i).get())) {
				auto cur = std::static_pointer_cast<BezierCurve>(scene->at(i));

				char buf[200];
				sprintf_s(buf, "%d. %s", counterCur, cur->name);
				if (ImGui::RadioButton(buf, !sceneObject_2.expired() && cur->GetId() == lockCurve->GetId())) {
					sceneObject_2 = cur;
				}

				counterCur++;
			}
		}
		ImGui::EndChild();
		ImGui::DragFloat("Radian##TransformSurfaceWindow", &r);
		if (ImGui::Button("Transform sceneObject_2 to sceneObject_1") &&
			!sceneObject_2.expired() && !sceneObject_1.expired()) {
			TransformSurfaceUsingCurve::Transform(sceneObject_1.lock().get(), sceneObject_2.lock().get(), r);

		}
	}

private:

};
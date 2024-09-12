#pragma once
#include "Figures/Bezier/BezierCurve.h"
#include "Figures/Bezier/Surface/BezierSurface.h"
#include "Figures/FigureFunctions/TransformSurfercUsingCurve.h"

class TransformSurfaceWindow {
public:
	const char* name = "Trans formSurface Window";
	bool open = false;
	float r = 0;
	std::weak_ptr<BezierSurface> surface;
	std::weak_ptr<BezierCurve> curve;
	void CreateMyWindow(Scene* scene) {
		if (!open)
			return;

		ImGui::Begin("Transform Surface using curve", &open);
		{
			Interferes(scene);
		}
		ImGui::End();
	}

	void Interferes(Scene* scene) {

		int counterSur = 0;
		int counterCur = 0;
		std::shared_ptr<BezierSurface> lockSurface;
		lockSurface = surface.lock();

		ImGui::BeginChild("Surfaces##TransformSurfaceWindow", ImVec2(ImGui::GetContentRegionAvail().x * 0.5f, 150), ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar);
		for (int i = 0; i < scene->Size(); i++) {
			if (dynamic_cast<BezierSurface*>( scene->at(i).get())) {
				auto sur = std::static_pointer_cast<BezierSurface>(scene->at(i));

				char buf[200];
				sprintf_s(buf, "%d. %s", counterSur, sur->name);
				if (ImGui::RadioButton(buf, !surface.expired() && sur->GetId() == lockSurface->GetId())) {
					surface = sur;
				}
				counterSur++;
			}
			
		}
		ImGui::EndChild();
		ImGui::SameLine();


		std::shared_ptr<BezierCurve> lockCurve;
		lockCurve = curve.lock();

		ImGui::BeginChild("Curve##TransformSurfaceWindow", ImVec2(0, 150), ImGuiChildFlags_None, ImGuiWindowFlags_HorizontalScrollbar);
		for (int i = 0; i < scene->Size(); i++) {
			if (dynamic_cast<BezierCurve*>(scene->at(i).get())) {
				auto cur = std::static_pointer_cast<BezierCurve>(scene->at(i));

				char buf[200];
				sprintf_s(buf, "%d. %s", counterCur, cur->name);
				if (ImGui::RadioButton(buf, !curve.expired() && cur->GetId() == lockCurve->GetId())) {
					curve = cur;
				}

				counterCur++;
			}
		}
		ImGui::EndChild();
		ImGui::DragFloat("Radian##TransformSurfaceWindow", &r);
		if (ImGui::Button("Transform curve to surface") &&
			!curve.expired() && !surface.expired()) {
			TransformSurfaceUsingCurve::Transform(surface.lock().get(), curve.lock().get(), r);

		}
	}

private:

};
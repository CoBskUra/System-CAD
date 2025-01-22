#pragma once
#include "Point.h"
#include <string>
#include "Container/FigureContainer.h"
#include "Helper/OpenGLHelper.h"
#include "Transformations/QuaternionRotationImGui.h"

class CenterPoint: public Point, public FigureContainer {

	enum TypeOfRefrencePoint {
		LocalMassCenter,
		GlobalMassCenter,
		CustomRefrencePoint
	};
protected:
	CenterPoint(const char* name, FigureType type):Point( type) {
		SetName(name);
		mouseLastPosition = glm::vec2{ 0 };
	}
public:
	CenterPoint(const char* name) : CenterPoint(name,  FigureType::CenterPoint){
	}

	CenterPoint() : CenterPoint("CenterPoint", FigureType::CenterPoint){
	}

	void  Draw(GLFWwindow* window, const Camera& camera) override {
		if (ContainerSize() > 0) {
			Update();
			Point::Draw(window, camera);
		}
	}

	void  ActiveImGui()override {
		ImGui::PushID("centerPoint");
		ImGui::BeginGroup(); {
			ImGui::Text("Control selected");
			// translation
			transpose->ActiveInterferes();
			if (ImGui::RadioButton("Local Mass Center", typeOfrefrencePoint == TypeOfRefrencePoint::LocalMassCenter))
			{
				typeOfrefrencePoint = LocalMassCenter;
				ResetValues();
			}
			if (ImGui::RadioButton("Global Mass Center", typeOfrefrencePoint == TypeOfRefrencePoint::GlobalMassCenter)) {

				typeOfrefrencePoint = GlobalMassCenter;
				ResetValues();
			}
			if (ImGui::RadioButton("Custom Mass Center", typeOfrefrencePoint == TypeOfRefrencePoint::CustomRefrencePoint))
			{
				typeOfrefrencePoint = CustomRefrencePoint;
				ResetValues();
			}
			if (typeOfrefrencePoint == TypeOfRefrencePoint::CustomRefrencePoint)
				ImGui::DragFloat3("Custom refPoint", glm::value_ptr(customRefrencePoint));

			RotationInterfers();
			ScaleInterfers();
			
		}
		ImGui::EndGroup();
		ImGui::PopID();
	}

	bool  Inputs(GLFWwindow* window, const Camera& camera) override {
		ImGuiIO& io = ImGui::GetIO();

		if (io.WantCaptureMouse)
			return false;

		if (firstClick && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
			firstClick = false;
			mouseLastPosition = OpenGLHelper::MousePositionOnScreen(window);
		}
		else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
			auto currentMousePosition = OpenGLHelper::MousePositionOnScreen(window);
			auto delta = currentMousePosition - mouseLastPosition;
			if (currentMousePosition != mouseLastPosition)
			{
				std::set<Figure* >::iterator iter;
				for (iter = selectedFigures.begin(); iter != selectedFigures.end(); iter++)
				{
					auto figure = (*iter);

					glm::vec4 mousePos{
						delta,
						0,
						0.0f
					};

					mousePos = camera.GetCameraMatrixInvers() * mousePos;
					//mousePos /= mousePos.w;
					figure->transpose->MoveObjectPosition(glm::vec3(mousePos));
				}
			}
			mouseLastPosition = currentMousePosition;
			return true;
		}
		else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
		{
			firstClick = true;
		}
		return false;
	}

	void  Update() override
	{
		if (somethingHasChange)
		{
			CalculateNewPosition();

			if (valueAdded || valueErased) {
				ResetValues();
			}

			FigureContainer::Update();
		}
	}


	void ScaleAlong(glm::vec3 scaleCenter, glm::vec3 scaleVec) override {
		std::set<Figure* >::iterator iter;
		for (iter = selectedFigures.begin(); iter != selectedFigures.end(); iter++)
		{
			(*iter)->ScaleAlong(scaleCenter, scaleVec / LastScaleVec);
		}
	}
protected:
	void SetObjectPosition(float x, float y, float z)  override {
		Transpose::SetObjectPosition(x, y, z);
		if (transpose_last.GetPosition() != transpose->GetPosition()) {
			std::set<Figure* >::iterator iter;
			for (iter = selectedFigures.begin(); iter != selectedFigures.end(); iter++)
			{
				(*iter)->transpose->
					MoveObjectPosition(transpose->GetPosition() - transpose_last.GetPosition());
			}
			transpose_last = *transpose;
		}
	}

	void ScaleInterfers() {
		ImGui::Text("Scale");

		if (ImGui::DragFloat("scale x##centerPoint", &scaleVec.x, 0.1f, M_ESP, M_FLOAT_MAX, "%.3f", ImGuiSliderFlags_AlwaysClamp) ||
			ImGui::DragFloat("scale y##centerPoint", &scaleVec.y, 0.1f, M_ESP, M_FLOAT_MAX, "%.3f", ImGuiSliderFlags_AlwaysClamp) ||
			ImGui::DragFloat("scale z##centerPoint", &scaleVec.z, 0.1f, M_ESP, M_FLOAT_MAX, "%.3f", ImGuiSliderFlags_AlwaysClamp)) {
			std::set<Figure* >::iterator iter;
			for (iter = selectedFigures.begin(); iter != selectedFigures.end(); iter++)
			{
				(*iter)->ScaleAlong(ReferencePoint(*(*iter)), scaleVec / LastScaleVec);
			}
			LastScaleVec = scaleVec;

			if (LastScaleVec.x == 0) { LastScaleVec.x = 1; }
			if (LastScaleVec.y == 0) { LastScaleVec.y = 1; }
			if (LastScaleVec.z == 0) { LastScaleVec.z = 1; }
		}
	}

	void RotationInterfers() {
		if (typeOfrefrencePoint == LocalMassCenter) {
			if (localQuaternion.ActiveInterferes()) {
				for (auto iter = selectedFigures.begin(); iter != selectedFigures.end(); iter++)
				{
					(*iter)->RotationAlong(localQuaternion.Get() * localQuaternion_last.Invers(), ReferencePoint(*(*iter)));
				}
			}
			if (localQuaternion_last != localQuaternion)
				localQuaternion_last = localQuaternion;
		}
		else {
			if (globalQuaternion.ActiveInterferes()) {
				for (auto iter = selectedFigures.begin(); iter != selectedFigures.end(); iter++)
				{
					(*iter)->RotationAlong(globalQuaternion.Get() * globalQuaternion_last.Invers(), ReferencePoint(*(*iter)));
				}
			}
			if (globalQuaternion_last != globalQuaternion)
				globalQuaternion_last = globalQuaternion;
		}
	}



private:
	bool firstClick = true;
	glm::vec2 mouseLastPosition;

	QuaternionRotationImGui localQuaternion;
	QuaternionRotationImGui localQuaternion_last;

	QuaternionRotationImGui globalQuaternion;
	QuaternionRotationImGui globalQuaternion_last;

	TypeOfRefrencePoint typeOfrefrencePoint{GlobalMassCenter};
	glm::vec3 customRefrencePoint{0,0,0};

	Transpose transpose_last;

	glm::vec3 scaleVec{ 1.0f };
	glm::vec3 LastScaleVec{ 1.0f };

	glm::vec3 ReferencePoint(const Figure& figure) {
		switch (typeOfrefrencePoint)
		{
		case CenterPoint::LocalMassCenter: return figure.transpose->GetPosition();
		case CenterPoint::GlobalMassCenter: return transpose->GetPosition();
		case CenterPoint::CustomRefrencePoint: return customRefrencePoint;
		default:
			return {};
		}
	}

	

	void MarkFigure(Figure* f) override{
		f->Mark();
	}

	

	void ResetValues() {
		localQuaternion.Reset();
		localQuaternion_last.Reset();

		globalQuaternion.Reset();
		globalQuaternion_last.Reset();

		LastScaleVec = scaleVec = glm::vec3(1, 1, 1);
		transpose_last = *transpose;
	}

	void CalculateNewPosition() {
		glm::vec3 position(0);
		for (auto iter = selectedFigures.begin(); iter != selectedFigures.end(); iter++)
		{
			position += (*iter)->transpose->GetPosition();
		}
		float number = selectedFigures.size();
		//transpose->SetObjectPosition(position / number);
		position = position / number;
		Figure::SetObjectPosition(position.x, position.y, position.z);
		transpose_last = *transpose;
	}
};
#pragma once
// zastanawiam siê by wyabstrachowaæ czêœci z centarPoint
// tak by mo¿na by³o ³atwo kontrolowaæ dodane obiekty do
// kontainera figur, mo¿na by te¿ pójœæ w ten sposób, ¿e 
// ka¿da funkcja ma w sobie klasê która odpowiada tylko za 
// specyficzny rodzaj wyœwietlania, kontrole imputu, itp.
// ale probelm wystêpuje wtedy parê problemów
// 1. trudniejsze debugowanie.
// 2. Koniecznoœæ specyfikacji unikalnych klas dla obiektów.
// 3. Jak nie unikalne to tworzymy misz masz kodu w funkcji i u¿ytych klas
// 4. Potrzebne by by³y wskazniki wiêc gdzieæ w porgramie musia³bym przechowywaæ dane.
// 5. Koniecznoœæ stworzenie fabryki, singeltona, podajnika.
#include "Point.h"
#include <string>
#include "Container/FigureContainer.h"
#include "Helper/OpenGLHelper.h"
#include "Transformations/QuaternionRotationImGui.h"

class Figure_ControlSelected : public Figure, public FigureContainer {
protected:
	Figure_ControlSelected(Shader* shader, const char* name, FigureType type) :Figure(shader, FigureType::CenterPoint) {
		SetName(name);
		mouseLastPosition = glm::vec2{ 0 };
	}
public:
	Figure_ControlSelected(Shader* shader, const char* name) : Figure_ControlSelected(shader, name,  FigureType::CenterPoint) {
	}

	Figure_ControlSelected(Shader* shader) : Figure_ControlSelected(shader, "Figure_ControlSelected",  FigureType::CenterPoint) {
	}

	void Draw(GLFWwindow* window, const Camera& camera) override {
		if(IsSomethingChange())
			Update();
	}

	void ActiveImGui()  override {
		ImGui::PushID("centerPoint");
		ImGui::BeginGroup(); {
			ImGui::Text("Control selected");
			// translation
			transpose->ActiveInterferes();
			if (ImGui::Checkbox("Local change", &localReference))
				ResetValues();
			RotationInterfers();
			ScaleInterfers();

		}
		ImGui::EndGroup();
		ImGui::PopID();
	}

	bool Inputs(GLFWwindow* window, const Camera& camera) override {
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

	void Update() override
	{
		CalculateNewPosition();

		if (valueAdded || valueErased) {
			ResetValues();

			valueAdded = false;
			valueErased = false;
		}

		somethingHasChange = false;
	}

protected:
	void SetObjectPosition(float x, float y, float z) override {
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
		if (localReference) {
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
	bool localReference = false;

	Transpose transpose_last;

	glm::vec3 scaleVec{ 1.0f };
	glm::vec3 LastScaleVec{ 1.0f };

	glm::vec3 ReferencePoint(const Figure& figure) {
		if (localReference)
			return figure.transpose->GetPosition();
		else
			return transpose->GetPosition();
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
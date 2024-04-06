#include "Manager.h"

Manager::Manager(Camera* camera, GLFWwindow* window):
	window(window), PointShader("point_vert.glsl", "point_frag.glsl"),
	CursorShader("point_vert.glsl", "point_frag.glsl"),
	centerPoint(&PointShader), cursor(&CursorShader)
{
	TorusShader = Shader("simple_vert.glsl", "simple_frag.glsl");
	currentCamera = camera;
	centerPoint.color = glm::vec4(1, 0, 0, 1);
}

void Manager::MenuInterferes()
{
	ImGui::Begin("Menu");
	{
		CreateFiguresInterfers();
		cursor.ActiveImGui();

		if (figuresVector.NumberOfActive() > 0) {
			ImGui::Text("Control selected");
			ImGui::Text("Rotation");
			if (ImGui::DragFloat("Rotate x", &angel.x, 0.1f, -M_PI, M_PI, "%.3f", ImGuiSliderFlags_AlwaysClamp)) {
				//auto axis = glm::normalize(centerPoint.transpose.GetPosition() - currentCamera->GetPosition());
				for (int i = 0; i < figuresVector.Size(); i++) {
					if (figuresVector.active[i])
						figuresVector.figures[i]->RotationAlong(glm::vec3(1, 0, 0), centerPoint.transpose.GetPosition(), angel.x - lastAngel.x);
				}
				lastAngel.x = angel.x;
			}

			if (ImGui::DragFloat("Rotate y", &angel.y, 0.1f, -M_PI, M_PI, "%.3f", ImGuiSliderFlags_AlwaysClamp)) {
				//auto axis = glm::normalize(centerPoint.transpose.GetPosition() - currentCamera->GetPosition());
				for (int i = 0; i < figuresVector.Size(); i++) {
					if (figuresVector.active[i])
						figuresVector.figures[i]->RotationAlong(glm::vec3(0, 1, 0), centerPoint.transpose.GetPosition(), angel.y - lastAngel.y);
				}
				lastAngel.y = angel.y;
			}

			if (ImGui::DragFloat("Rotate z", &angel.z, 0.1f, -M_PI, M_PI, "%.3f", ImGuiSliderFlags_AlwaysClamp)) {
				//auto axis = glm::normalize(centerPoint.transpose.GetPosition() - currentCamera->GetPosition());
				for (int i = 0; i < figuresVector.Size(); i++) {
					if (figuresVector.active[i])
						figuresVector.figures[i]->RotationAlong(glm::vec3(0, 0, 1), centerPoint.transpose.GetPosition(), angel.z - lastAngel.z);
				}
				lastAngel.z = angel.z;
			}

			ImGui::Text("Scale");

			if (ImGui::DragFloat("scale x##centerPoint", &scaleVec.x, 0.1f, M_ESP, M_FLOAT_MAX,"%.3f", ImGuiSliderFlags_AlwaysClamp) ||
				ImGui::DragFloat("scale y##centerPoint", &scaleVec.y, 0.1f, M_ESP, M_FLOAT_MAX, "%.3f", ImGuiSliderFlags_AlwaysClamp) ||
				ImGui::DragFloat("scale z##centerPoint", &scaleVec.z, 0.1f, M_ESP, M_FLOAT_MAX, "%.3f", ImGuiSliderFlags_AlwaysClamp)) {
				for (int i = 0; i < figuresVector.Size(); i++) {
					if (figuresVector.active[i])
						figuresVector.figures[i]->ScaleAlong(centerPoint.transpose.GetPosition(), scaleVec / LastScaleVec);
				}
				LastScaleVec = scaleVec;

				if (LastScaleVec.x == 0) { LastScaleVec.x = 1; }
				if (LastScaleVec.y == 0) { LastScaleVec.y = 1; }
				if (LastScaleVec.z == 0) { LastScaleVec.z = 1; }

			}
		}
		SelectableList();
		DeleteSelected();
		
	}
	ImGui::End();
}

void Manager::Draw()
{
	for (int i = 0; i < figuresVector.Size(); i++)
	{
		if (figuresVector.figures[i]->shader != currentShader)
		{
			currentShader = figuresVector.figures[i]->shader;
			currentShader->Activate();
			currentCamera->SaveMatrixToShader(*currentShader);
		}

		if(currentCamera->HasBeenUpdated())
			currentCamera->SaveMatrixToShader(*currentShader);

		if (figuresVector.active[i])
		{
			figuresVector.figures[i]->Mark();
			if(figuresVector.NumberOfActive() < 2)
				figuresVector.figures[i]->ActiveImGui(); // prze¿uæ do interfersu ale najpierw lepsze baza na figury
														// taka bym mia³ dostêp do aktywnych w czasie O(1) 
		}
		figuresVector.figures[i]->Draw();

		if (figuresVector.active[i])
		{
			figuresVector.figures[i]->UnMark();
		}
	}

	if (figuresVector.NumberOfActive() > 0) {
		if (currentShader != &PointShader) { PointShader.Activate(); }

		currentCamera->SaveMatrixToShader(PointShader);
		centerPoint.Draw();
		UpdateCenterPoint();

		if (currentShader != &PointShader) { currentShader->Activate(); }
	}

	CursorShader.Activate();
	currentCamera->SaveMatrixToShader(PointShader);
	cursor.Draw();
	if(currentShader != nullptr)
		currentShader->Activate();

}

Manager::~Manager()
{
}

int Manager::TheClosetFigureToMouse(const char* figureType)
{

	
	auto castMousePos = OpenGLHelper::MousePositionOnScreen(window);
	float minLength = M_FLOAT_MAX;
	int id = -1;

	for (int i = 0; i < figuresVector.Size(); i++) {
		if (strcmp(figuresVector.figures[i]->GetType(), figureType) == 0) {
			auto posOnScreen = figuresVector.figures[i]->PositionOnScreen(*currentCamera);
			float length = powf(posOnScreen.x - castMousePos.x, 2) + powf(posOnScreen.y - castMousePos.y, 2);
			if (minLength > length)
			{
				id = i;
				minLength = length;
			}
		}
	}

	if (minLengthFromMouse < minLength)
		id = -1;

	return id;
}

void Manager::CreateFiguresInterfers()
{
	if (ImGui::Button("Create Torus", ImVec2(100, 20))) {

		Torus* torus = new Torus(&TorusShader, "Torus");
		torus->transpose.SetObjectPosition(cursor.transpose.GetPosition());
		figuresVector.AddFigure(torus);
	}

	if (ImGui::Button("Create Point", ImVec2(100, 20))) {

		Point* point = new Point(&PointShader, "Point");
		point->transpose.SetObjectPosition(cursor.transpose.GetPosition());
		figuresVector.AddFigure(point);
	}
}

void Manager::SelectableList()
{
	for (int i = 0; i < figuresVector.Size(); i++)
	{
		char buf[100];
		sprintf_s(buf, "%d. %s", i, figuresVector.names[i]);
		if (ImGui::Selectable(buf, figuresVector.active[i])) {
			figuresVector.ChangeActiveState(i);
		}
	}
}

void Manager::DeleteSelected()
{
	if (figuresVector.NumberOfActive() > 0)
	{
		if (ImGui::Button("Delete", ImVec2(50, 20))) {
			for (int i = figuresVector.Size() - 1; i >= 0; i--)
				if (figuresVector.active[i])
					figuresVector.DeleteFigure(i);
		}
	}
}

void Manager::UpdateCenterPoint()
{
	glm::vec3 position(0);
	for (int i = 0; i < figuresVector.Size(); i++)
	{
		if (figuresVector.active[i])
			position += figuresVector.figures[i]->transpose.GetPosition();
	}
	int activeCount = figuresVector.NumberOfActive();
	centerPoint.transpose.SetObjectPosition(position.x/ activeCount, position.y/ activeCount, position.z/ activeCount);
}

void Manager::ProcesInput()
{
	ImGuiIO& io = ImGui::GetIO();

	if (io.WantCaptureKeyboard)
		return;

	if (mouseLeftFirstClick && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {

		int id = TheClosetFigureToMouse("Point");
		if (id >= 0) {
			figuresVector.ChangeActiveState(id);
		}
		
		mouseLeftFirstClick = false;

	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
		mouseLeftFirstClick = true;
	}

	cursor.Inputs(window, *currentCamera);
}



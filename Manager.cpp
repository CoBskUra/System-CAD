#include "Manager.h"
#include "StaticShaders.h"

Manager::Manager(Camera* camera, GLFWwindow* window):
	window(window)
{
	StaticShaders::Init();
	currentCamera = camera;
	centerPoint.SetColor( glm::vec4(1, 0, 0, 1));
	cursor.transpose->SetObjectPosition(0.5f, 1.2f, -1.0f);
}

void Manager::MenuInterferes()
{
	ImGui::Begin("Menu");
	{
		CreateFiguresInterfers();
		cursor.ActiveImGui();
		centerPoint.ActiveImGui();
		SelectableList();
	}
	ImGui::End();
}

void Manager::Draw()
{
	for (int i = 0; i < figuresVector.Size(); i++)
	{
		if (figuresVector.active[i])
		{
			ImGui::PushID(i);
			figuresVector.figures[i]->ActiveImGui(); // prze¿uæ do interfersu ale najpierw lepsze baza na figury
														// taka bym mia³ dostêp do aktywnych w czasie O(1) 
			ImGui::PopID();

			ImGui::Separator();
			ImGui::NewLine();
			ImGui::Separator();
		}

		figuresVector.figures[i]->Draw(window, *currentCamera);
	}

	centerPoint.Draw(window, *currentCamera);
	cursor.Draw(window, *currentCamera);
}

Manager::~Manager()
{
}


int Manager::TheClosetFigureToMouse(FigureType figureType)
{
	auto castMousePos = OpenGLHelper::MousePositionOnScreen(window);
	float minLength = M_FLOAT_MAX;
	int id = -1;

	for (int i = 0; i < figuresVector.Size(); i++) {
		if (dynamic_cast<FigureContainer*>(figuresVector.figures[i]))
			continue;
		if (figuresVector.figures[i]->GetType() == figureType || FigureType::Any == figureType) {

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
	if (ImGui::Button("Torus", ImVec2(100, 20))) {

		Torus* torus = new Torus();
		torus->transpose->SetObjectPosition(cursor.transpose->GetPosition());
		figuresVector.AddFigure(torus);
	}
	ImGui::SameLine();
	if (ImGui::Button("Point", ImVec2(100, 20))) {

		Point* point = new Point();
		point->transpose->SetObjectPosition(cursor.transpose->GetPosition());
		figuresVector.AddFigure(point);
	}
	ImGui::SameLine();
	if (ImGui::Button("Bezier C0", ImVec2(100, 20))) {
		BezierC0* Bezier = new BezierC0();
		Bezier->transpose->SetObjectPosition(cursor.transpose->GetPosition());
		figuresVector.AddFigure(Bezier);
		Bezier->Add(centerPoint);
	}

	if (ImGui::Button("Bezier C2", ImVec2(100, 20))) {
		BezierC2* Bezier = new BezierC2();
		Bezier->transpose->SetObjectPosition(cursor.transpose->GetPosition());
		figuresVector.AddFigure(Bezier);
		Bezier->Add(centerPoint);
	}

	ImGui::SameLine();
	if (ImGui::Button("Bezier Interpolated", ImVec2(100, 20))) {
		BezierInterpolated* Bezier = new BezierInterpolated();
		Bezier->transpose->SetObjectPosition(cursor.transpose->GetPosition());
		figuresVector.AddFigure(Bezier);
		Bezier->Add(centerPoint);
	}
}

void Manager::SelectableList()
{
	ImVec2 size(150, 20);
	for (int i = 0; i < figuresVector.Size(); i++)
	{
		ImGui::PushID(i);
		if (ImGui::Button("X", ImVec2(20, 20))) {
			figuresVector.DeleteFigure(i);
			ImGui::PopID();
			return;
		}
		ImGui::SameLine();


		if (ImGui::RadioButton("", figuresVector.active[i])) {
			figuresVector.ChangeActiveState(i);
		}

		ImGui::SameLine();

		char buf[100];
		sprintf_s(buf, "%d. %s", i, figuresVector.names[i]);

		if (ImGui::Selectable(buf, centerPoint.Contain(figuresVector.figures[i]))) {
			Select(i);
		}

		ImGui::PopID();
	}
}




void Manager::Select(int i)
{
	if (!centerPoint.Contain(figuresVector.figures[i])) {
		centerPoint.Add(figuresVector.figures[i]);
		for (auto iter = figuresVector.activeFigureContainers.begin(); iter != figuresVector.activeFigureContainers.end(); iter++) {
			iter->second->Add(figuresVector.figures[i]);
		}
	}
	else {
		centerPoint.Erase(figuresVector.figures[i]);
	}
}

void Manager::ProcesInput()
{
	ImGuiIO& io = ImGui::GetIO();

	if (io.WantCaptureKeyboard)
		return;

	if (mouseLeftFirstClick && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {

		int id = TheClosetFigureToMouse(FigureType::Any);
		if (id >= 0) {
			Select(id);
		}
		mouseLastPosition = OpenGLHelper::MousePositionOnScreen(window);
		mouseLeftFirstClick = false;
	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
		mouseLeftFirstClick = true;
	}

	for (int i = 0; i < figuresVector.Size(); i++)
		figuresVector.at(i)->Inputs(window, *currentCamera);

	centerPoint.Inputs(window, *currentCamera);
	cursor.Inputs(window, *currentCamera);
}



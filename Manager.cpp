#include "Manager.h"

Manager::Manager(Camera* camera, GLFWwindow* window):
	window(window), stereoscopicView(*camera)
{
	StaticShaders::Init();
	currentCamera = camera;
	mainCamera = camera;
	centerPoint.SetUnmarkColor( glm::vec4(1, 0, 0, 1));
	cursor.transpose->SetObjectPosition(0.5f, 1.2f, -1.0f);
}

void Manager::MenuInterferes()
{
	ImGui::Begin("Menu");
	{
		if (ImGui::RadioButton("Turn on stereoscopic view", stereoscopicView.turnOn))
		{
			stereoscopicView.turnOn = !stereoscopicView.turnOn;
			if (stereoscopicView.turnOn) {
				stereoscopicView = *currentCamera;
				currentCamera = &stereoscopicView;
				currentCamera->updateMatrixes();
			}
			else {
				*mainCamera = *currentCamera;
				currentCamera = mainCamera;
				currentCamera->updateMatrixes();
			}
		}
		CreateFiguresInterfers();
		cursor.ActiveImGui();
		centerPoint.ActiveImGui();
		SelectableList();
	}
	ImGui::End();

	if (figuresVector.NumberOfActive() > 0) {
		ImGui::Begin("Objects");
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
		}
		ImGui::End();
	}

	ImGui::Begin("Camera Control");
	currentCamera->ActiveInterferes();
	ImGui::End();
}

void Manager::Draw()
{
	if (!stereoscopicView.turnOn) {

		for (int i = 0; i < figuresVector.Size(); i++)
		{
			figuresVector.figures[i]->Draw(window, *currentCamera);
			//stereoscopicView.Draw(window, *currentCamera, figuresVector.figures[i]);
		}

		centerPoint.Draw(window, *currentCamera);
		cursor.Draw(window, *currentCamera);
		infinityGrid.Draw(window, *currentCamera);
	}
	else {
		//stereoscopicView = (*mainCamera);
		stereoscopicView.LeftEyeSeting();
		for (int i = 0; i < 2; i++) {
			if(i == 0)
				glColorMask(true, false, false, false);
			else
				glColorMask(false, false, true, false);

			for (int i = 0; i < figuresVector.Size(); i++)
			{
				glm::vec4 oldColor = figuresVector.figures[i]->SetUnmarkColor({ 1, 1, 1, 1 });
				figuresVector.figures[i]->Draw(window, *currentCamera);
				figuresVector.figures[i]->SetUnmarkColor(oldColor);
				//stereoscopicView.Draw(window, *currentCamera, figuresVector.figures[i]);
			}

			centerPoint.Draw(window, *currentCamera);
			cursor.Draw(window, *currentCamera);
			infinityGrid.Draw(window, *currentCamera);

			glClear(GL_DEPTH_BUFFER_BIT);
			stereoscopicView.RighteEyeSeting();
		}
		glColorMask(true, true, true, true);
		stereoscopicView.MonoEyeSetting();
		//currentCamera = mainCamera;
	}
	
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

	//ImGui::SameLine();
	if (ImGui::Button("Bezier surfec C0", ImVec2(150, 20))) {
		BezierSurfaceC0* Bezier = new BezierSurfaceC0();
		Bezier->transpose->SetObjectPosition(cursor.transpose->GetPosition());
		Bezier->figureVector = &figuresVector;
		figuresVector.AddFigure(Bezier);
		figuresVector.ChangeActiveState(figuresVector.Size() - 1);
	}


	ImGui::SameLine();
	if (ImGui::Button("Bezier surfec C2", ImVec2(150, 20))) {
		BezierSurfaceC2* Bezier = new BezierSurfaceC2();
		Bezier->transpose->SetObjectPosition(cursor.transpose->GetPosition());
		Bezier->figureVector = &figuresVector;
		figuresVector.AddFigure(Bezier);
		figuresVector.ChangeActiveState(figuresVector.Size() - 1);
	}
}

void Manager::SelectableList()
{
	ImVec2 size(150, 20);
	for (int i = 0; i < figuresVector.Size(); i++)
	{
		ImGui::PushID(i);
		// delete figure
		if (!figuresVector.at(i)->HaveOwner()) {
			if (ImGui::Button("X", ImVec2(20, 20))) {
				figuresVector.DeleteFigure(i);
				ImGui::PopID();
				return;
			}
			ImGui::SameLine();
		}


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
			(*iter)->Add(figuresVector.figures[i]);
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
	currentCamera->Inputs(window);
}



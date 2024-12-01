#include "Manager.h"

Manager::Manager(Camera* camera, GLFWwindow* window):
	window(window), stereoscopicView(*camera)
{
	StaticShaders::Init();
	currentCamera = camera;
	mainCamera = camera;
	centerPoint.SetUnmarkColor( glm::vec4(1, 0, 0, 1));
	cursor.transpose->SetObjectPosition(0.5f, 1.2f, -1.0f);
	fileDialog.SetTitle("File browser");
}

void Manager::MenuInterferes()
{
	ImGui::Begin("test tekstury"); {
		heightGenerator.Interfers(sher_ptrScene.get());
	}
	ImGui::End();

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
		LoadScene();
		ImGui::SameLine();
		if (ImGui::Button("ClearScene")) {
			ClearScene();
		}

		SaveScene();
		
		TransformSurfaceUsingCurve();
		IntersectionWindow();

		CreateFiguresInterfers();
		cursor.ActiveImGui();
		centerPoint.ActiveImGui();
		SelectableList(FigureFilter());
	}
	ImGui::End();

	if (sher_ptrScene->NumberOfActive() > 0) {
		ImGui::Begin("Objects");
		for (int i = 0; i < sher_ptrScene->Size(); i++)
		{
			if (sher_ptrScene->active[i])
			{
				ImGui::PushID(i);
				sher_ptrScene->at(i)->ActiveImGui(); // prze¿uæ do interfersu ale najpierw lepsze baza na figury
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
	ImGui::ShowDemoWindow();
}

void Manager::Draw()
{
	if (!stereoscopicView.turnOn) {

		for (int i = 0; i < sher_ptrScene->Size(); i++)
		{
			sher_ptrScene->at(i)->Draw(window, *currentCamera);
			//stereoscopicView.Draw(window, *currentCamera, sher_ptrScene->at(i));
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

			for (int i = 0; i < sher_ptrScene->Size(); i++)
			{
				glm::vec4 oldColor = sher_ptrScene->at(i)->SetUnmarkColor({ 1, 1, 1, 1 });
				sher_ptrScene->at(i)->Draw(window, *currentCamera);
				sher_ptrScene->at(i)->SetUnmarkColor(oldColor);
				//stereoscopicView.Draw(window, *currentCamera, sher_ptrScene->at(i));
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

	for (int i = 0; i < sher_ptrScene->Size(); i++) {
		if (dynamic_cast<FigureContainer*>(sher_ptrScene->at(i).get()))
			continue;
		if (sher_ptrScene->at(i)->GetType() == figureType || FigureType::Any == figureType) {

			auto posOnScreen = sher_ptrScene->at(i)->PositionOnScreen(*currentCamera);
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
		std::shared_ptr<Torus> torus = std::make_shared<Torus>();
		torus->transpose->SetObjectPosition(cursor.transpose->GetPosition());
		sher_ptrScene->AddFigure(torus);
	}
	ImGui::SameLine();
	if (ImGui::Button("Point", ImVec2(100, 20))) {

		std::shared_ptr <Point> point = std::make_shared<Point>();
		point->transpose->SetObjectPosition(cursor.transpose->GetPosition());
		sher_ptrScene->AddFigure(point);
	}
	ImGui::SameLine();
	if (ImGui::Button("Bezier C0", ImVec2(100, 20))) {
		std::shared_ptr<BezierC0> Bezier = std::make_shared<BezierC0>();
		Bezier->transpose->SetObjectPosition(cursor.transpose->GetPosition());
		sher_ptrScene->AddFigure(Bezier);
		Bezier->Add(centerPoint);
	}

	if (ImGui::Button("Bezier C2", ImVec2(100, 20))) {
		std::shared_ptr<BezierC2> Bezier = std::make_shared<BezierC2>();
		Bezier->transpose->SetObjectPosition(cursor.transpose->GetPosition());
		sher_ptrScene->AddFigure(Bezier);
		Bezier->Add(centerPoint);
	}

	ImGui::SameLine();
	if (ImGui::Button("Bezier Interpolated", ImVec2(100, 20))) {
		std::shared_ptr<BezierInterpolated> Bezier = std::make_shared<BezierInterpolated>();
		Bezier->transpose->SetObjectPosition(cursor.transpose->GetPosition());
		sher_ptrScene->AddFigure(Bezier);
		Bezier->Add(centerPoint);
	}

	ImGui::SameLine();
	if (ImGui::Button("Bezier surfec C0", ImVec2(150, 20))) {
		std::shared_ptr<BezierSurfaceC0> Bezier = std::make_shared<BezierSurfaceC0>();
		Bezier->transpose->SetObjectPosition(cursor.transpose->GetPosition());
		Bezier->refrenceScene = sher_ptrScene.get();
		sher_ptrScene->AddFigure(Bezier);
		sher_ptrScene->ChangeActiveState(sher_ptrScene->Size() - 1);
	}


	//ImGui::SameLine();
	if (ImGui::Button("Bezier surfec C2", ImVec2(150, 20))) {
		std::shared_ptr<BezierSurfaceC2> Bezier = std::make_shared<BezierSurfaceC2>();
		Bezier->transpose->SetObjectPosition(cursor.transpose->GetPosition());
		Bezier->refrenceScene = sher_ptrScene.get();
		sher_ptrScene->AddFigure(Bezier);
		sher_ptrScene->ChangeActiveState(sher_ptrScene->Size() - 1);
	}

	ImGui::SameLine();
	if (ImGui::Button("Gregory Patch", ImVec2(150, 20))) {
		std::shared_ptr<GregoryPatch> gregory = 
			std::make_shared<GregoryPatch>(sher_ptrScene.get());
		sher_ptrScene->AddFigure(gregory);
	}
}

FigureType Manager::FigureFilter()
{
	auto figuresTypes = sher_ptrScene->GetTypeOfFiguresOnScene();
	static int item_current = 0;
	if (item_current > figuresTypes.size() - 1)
		item_current = 0;
	std::vector<const char*> names;
	for (FigureType f : figuresTypes) {
		names.push_back(ToString(f));
	}
	ImGui::Combo("FiguresTypes", &item_current, names.data(), names.size());

	return figuresTypes[item_current];
}

void Manager::SelectableList(FigureType figureType)
{
	ImVec2 size(150, 20);
	for (int i = 0; i < sher_ptrScene->Size(); i++)
	{
		if (figureType != FigureType::Any && sher_ptrScene->at(i)->GetType() != figureType)
			continue;
		ImGui::PushID(i);
		// delete figure
		if (!sher_ptrScene->IsDeleteAble(i)) {
			if (ImGui::Button("X", ImVec2(20, 20))) {
				sher_ptrScene->DeleteFigureAt(i);
				ImGui::PopID();
				return;
			}
			ImGui::SameLine();
		}


		if (ImGui::RadioButton("", sher_ptrScene->active[i])) {
			sher_ptrScene->ChangeActiveState(i);
		}

		ImGui::SameLine();

		char buf[200];
		sprintf_s(buf, "%d. %s", i, sher_ptrScene->at(i)->name);
		if (ImGui::Selectable(buf, centerPoint.Contain(sher_ptrScene->at(i).get()))) {
			SelectUnselect(i);
		}

		ImGui::PopID();
	}
}




void Manager::SelectUnselect(int i)
{
	if (!centerPoint.Contain(sher_ptrScene->at(i).get())) {
		centerPoint.Add(sher_ptrScene->at(i).get());
		for (auto iter = sher_ptrScene->activeFigureContainers.begin(); iter != sher_ptrScene->activeFigureContainers.end(); iter++) {
			(*iter)->Add(sher_ptrScene->at(i).get());
		}
	}
	else {
		centerPoint.Erase(sher_ptrScene->at(i).get());
	}
}

void Manager::LoadScene()
{
	if (ImGui::Button("load scene##ManagerLoadScene"))
	{
		fileDialog.SetTypeFilters({ ".json" });
		fileDialog.Open();
	}
	fileDialog.Display();

	if (fileDialog.HasSelected())
	{
		try {
			SceneSerializer sceneSerializer{};
			sher_ptrScene = sceneSerializer.LoadScene(
				fileDialog.GetSelected().string().c_str(),
				move(sher_ptrScene));
		}
		catch (...) {
			std::cout << "Unable to loadScene";
		}
		fileDialog.ClearSelected();
	}
}

void Manager::SaveScene()
{
	ImGui::InputText("Save Scene name", pathDestination, sizeof(pathDestination));
	SceneSerializer sceneSerializer{};
	if (ImGui::Button("Save##ManagerSceneScene")) {
		try {
			sceneSerializer.SaveScene(*sher_ptrScene.get(), pathDestination);
		}
		catch (...) {
			std::cout << "Unable to saveScene";
		}
	}
}

bool Manager::UnselectAllShortCut()
{
	if (1 == glfwGetKey(window, GLFW_KEY_LEFT_ALT) && 1 == glfwGetKey(window, GLFW_KEY_U)) {
		for (int i = 0; i < sher_ptrScene->Size(); i++)
			if(centerPoint.Contain(sher_ptrScene->at(i).get()) ) {
				SelectUnselect(i);
		}
		return true;
	}
	return false;
}

bool Manager::MergeFigures()
{
	if (1 == glfwGetKey(window, GLFW_KEY_LEFT_ALT) && 1 == glfwGetKey(window, GLFW_KEY_M))
	{
		MergePoints merge{ sher_ptrScene.get() };
		return merge.Merge(&centerPoint);
	}
	return false;
}

void Manager::ClearScene()
{
	sher_ptrScene->DeleteAll();
}

void Manager::TransformSurfaceUsingCurve()
{
	if(ImGui::Button("Transform surface with curve##Manager"))
		transformSurface.open = true;
	transformSurface.CreateMyWindow(sher_ptrScene.get());
}

void Manager::IntersectionWindow()
{
	intersectionWindow.camera = currentCamera;
	intersectionWindow.window = window;
	intersectionWindow.cursor = &cursor;
	if (ImGui::Button("Intersection##Manager"))
		intersectionWindow.open = true;
	intersectionWindow.CreateMyWindow(sher_ptrScene.get());
}

void Manager::SelectManyWithMouse(FigureType figureType)
{
	auto castMousePos = OpenGLHelper::MousePositionOnScreen(window);
	float minLength = M_FLOAT_MAX;
	float min_x = mouseFirstClickPosition.x < castMousePos.x ? mouseFirstClickPosition.x : castMousePos.x;
	float max_x = mouseFirstClickPosition.x > castMousePos.x ? mouseFirstClickPosition.x : castMousePos.x;

	float min_y = mouseFirstClickPosition.y < castMousePos.y ? mouseFirstClickPosition.y : castMousePos.y;
	float max_y = mouseFirstClickPosition.y > castMousePos.y ? mouseFirstClickPosition.y : castMousePos.y;

	for (int i = 0; i < sher_ptrScene->Size(); i++) {
		if (dynamic_cast<FigureContainer*>(sher_ptrScene->at(i).get()))
			continue;
		if (sher_ptrScene->at(i)->GetType() == figureType || FigureType::Any == figureType) {

			auto posOnScreen = sher_ptrScene->at(i)->PositionOnScreen(*currentCamera);
			
			if (min_x <= posOnScreen.x && posOnScreen.x < max_x &&
				min_y <= posOnScreen.y && posOnScreen.y < max_y)
			{
				if (!centerPoint.Contain(sher_ptrScene->at(i).get()))
					SelectUnselect(i);
			}
		}
	}
}


void Manager::ProcessInput()
{

	UnselectAllShortCut();
	MergeFigures();
	// Ignor if focus at imgui window
	ImGuiIO& io = ImGui::GetIO();

	if (io.WantCaptureKeyboard)
		return;

	if(mouseLeftFirstClick)
		mouseFirstClickPosition = OpenGLHelper::MousePositionOnScreen(window);

	bool canMove = true;

	if (!mouseLeftFirstClick && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS &&
		glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
	{
		SelectManyWithMouse(FigureType::Point);
		
		mouseLastPosition = OpenGLHelper::MousePositionOnScreen(window);
		mouseLeftFirstClick = false;
		canMove = false;
	}
	else if (mouseLeftFirstClick && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {

		int id = TheClosetFigureToMouse(FigureType::Any);
		if (id >= 0) {
			SelectUnselect(id);
		}
		mouseLastPosition = OpenGLHelper::MousePositionOnScreen(window);
		mouseLeftFirstClick = false;
	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
		mouseLeftFirstClick = true;
		mouseFirstClickPosition = OpenGLHelper::MousePositionOnScreen(window);;
	}

	for (int i = 0; i < sher_ptrScene->Size(); i++)
		sher_ptrScene->at(i)->Inputs(window, *currentCamera);


	
	cursor.Inputs(window, *currentCamera);
	currentCamera->Inputs(window);
	if(canMove)
		centerPoint.Inputs(window, *currentCamera);
}
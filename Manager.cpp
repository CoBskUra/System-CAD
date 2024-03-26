#include "Manager.h"
#include "Point.h"

Manager::Manager(Camera* camera, GLFWwindow* window): window(window)
{
	TorusShader = Shader("simple_vert.glsl", "simple_frag.glsl");
	PointShader = Shader("point_vert.glsl", "point_frag.glsl");
	currentCamera = camera;
}

void Manager::MenuInterferes()
{
	ImGui::Begin("Menu");
	{
		if (ImGui::Button("Create Torus", ImVec2(100, 20))) {

			Torus* torus = new Torus(&TorusShader, "Torus");
			figuresVector.AddFigure(torus);
		}

		if (ImGui::Button("Create Point", ImVec2(100, 20))) {

			Point* point = new Point(&PointShader, "Point");
			figuresVector.AddFigure(point);
		}

		for (int i = 0; i < figuresVector.Size(); i++)
		{
			char buf[100];
			sprintf_s(buf, "%d. %s", i, figuresVector.names[i]);
			if (ImGui::Selectable(buf, figuresVector.active[i])) {
				figuresVector.ChangeActiveState(i);
			}
		}


		if (figuresVector.NumberOfActive() > 0)
		{
			if (ImGui::Button("Delete", ImVec2(50, 20))) {
				for(int i = 0; i < figuresVector.Size(); i++ )
					if(figuresVector.active[i])
						figuresVector.DeleteFigure(i);
			}
		}
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
				figuresVector.figures[i]->ActiveImGui();
		}
		figuresVector.figures[i]->Draw();
		figuresVector.figures[i]->UnMark();
	}

}

Manager::~Manager()
{
}

int Manager::TheClosetFigureToMouse(const char* figureType)
{
	glm::dvec2 mousPos;
	glfwGetCursorPos(window, &mousPos.x, &mousPos.y);
	glm::ivec2 windowSize;
	glfwGetWindowSize(window, &windowSize.x, &windowSize.y);
	/*auto windowSize = ImGui::GetWindowSize();
	auto mousePos = ImGui::GetMousePos();*/

	auto castMousePos = ImVec2(mousPos.x - windowSize.x * 0.5f, -mousPos.y + windowSize.y * 0.5f);
	castMousePos = ImVec2(castMousePos.x / (windowSize.x * 0.5f), castMousePos.y / (windowSize.y * 0.5f));
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

void Manager::ProcesInput()
{
	ImGuiIO& io = ImGui::GetIO();

	if (io.WantCaptureKeyboard)
		return;

	if (mouseLeftFirstClick && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {

		int id = TheClosetFigureToMouse("Point");
		if (id >= 0)
			figuresVector.ChangeActiveState(id);
		mouseLeftFirstClick = false;

	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
		mouseLeftFirstClick = true;
	}
}

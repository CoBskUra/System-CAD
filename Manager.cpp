#include "Manager.h"

Manager::Manager(Camera* camera, GLFWwindow* window):
	window(window), PointShader("point_vert.glsl", "point_frag.glsl"),
	CursorShader("point_vert.glsl", "point_frag.glsl"),
	centerPoint(&PointShader), cursor(&CursorShader)
{
	TorusShader = Shader({
		{"simple_vert.glsl", "VERTEX", GL_VERTEX_SHADER} ,
		{"simple_frag.glsl", "FRAGMENT", GL_FRAGMENT_SHADER} ,
		/*{"bezier3d_tc.glsl", "TESELATION_CONTROL", GL_TESS_CONTROL_SHADER},
		{"bezier3d_te.glsl", "TESELATION_CONTROL", GL_TESS_EVALUATION_SHADER}*/
		});//("simple_vert.glsl", "simple_frag.glsl");
	BezierShader = Shader({
		{"bezier3d_vert.glsl", "VERTEX", GL_VERTEX_SHADER} ,
		{"bezier3d_frag.glsl", "FRAGMENT", GL_FRAGMENT_SHADER} ,
		/*{"bezier3d_tc.glsl", "TESELATION_CONTROL", GL_TESS_CONTROL_SHADER},
		{"bezier3d_te.glsl", "TESELATION_CONTROL", GL_TESS_EVALUATION_SHADER}*/
		});
	currentCamera = camera;
	centerPoint.color = glm::vec4(1, 0, 0, 1);
}

void Manager::MenuInterferes()
{
	ImGui::Begin("Menu");
	{
		CreateFiguresInterfers();
		cursor.ActiveImGui();
		centerPoint.ActiveImGui();
		
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

		if (currentCamera->HasBeenUpdated())
			currentCamera->SaveMatrixToShader(*currentShader);

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

		figuresVector.figures[i]->Draw();
	}

	DrawSpecialFigure(centerPoint);
	DrawSpecialFigure(cursor);
}

Manager::~Manager()
{
}

void Manager::DrawSpecialFigure(Figure& figure) {
	if( currentShader != figure.shader) figure.shader->Activate();
	currentCamera->SaveMatrixToShader(*figure.shader);
	figure.Draw();
	if (currentShader != nullptr) currentShader->Activate();
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

		Torus* torus = new Torus(&TorusShader);
		torus->transpose.SetObjectPosition(cursor.transpose.GetPosition());
		figuresVector.AddFigure(torus);
	}
	ImGui::SameLine();
	if (ImGui::Button("Create Point", ImVec2(100, 20))) {

		Point* point = new Point(&PointShader);
		point->transpose.SetObjectPosition(cursor.transpose.GetPosition());
		figuresVector.AddFigure(point);
	}
	ImGui::SameLine();
	if (ImGui::Button("Create Bezier Curve", ImVec2(100, 20))) {
		BezierCurve* bezierCurve = new BezierCurve(&BezierShader);
		bezierCurve->transpose.SetObjectPosition(cursor.transpose.GetPosition());
		figuresVector.AddFigure(bezierCurve);
	}
}

void Manager::SelectableList()
{
	ImVec2 size(150, 20);
	for (int i = 0; i < figuresVector.Size(); i++)
	{
		ImGui::PushID(i);
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

void Manager::DeleteSelected()
{
	if (figuresVector.NumberOfActive() > 0)
	{
		if (ImGui::Button("Delete", ImVec2(50, 20))) {
			for (int i = figuresVector.Size() - 1; i >= 0; i--)
				if (figuresVector.active[i]) {
					centerPoint.Erase(figuresVector.figures[i]);
					figuresVector.DeleteFigure(i);
				}
		}
	}
}


void Manager::Select(int i)
{
	if (!centerPoint.Contain(figuresVector.figures[i])) {
		centerPoint.Add(figuresVector.figures[i]);
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

		int id = TheClosetFigureToMouse("Point");
		if (id >= 0) {
			Select(id);
		}
		
		mouseLeftFirstClick = false;

	}
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
		mouseLeftFirstClick = true;
	}

	cursor.Inputs(window, *currentCamera);
}



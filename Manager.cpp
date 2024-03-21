#include "Manager.h"

Manager::Manager(Camera* camera)
{
	TorusShader = Shader("simple_vert.glsl", "simple_frag.glsl");
	currentCamera = camera;
}

void Manager::MenuInterferes()
{
	ImGui::Begin("Menu");
	{
		ImGui::ListBox("listbox", &selectedTorus, figuresVector.names.data(), figuresVector.names.size(), 4);
		if (ImGui::Button("Create Torus", ImVec2(50, 20))) {

			Torus* torus = new Torus(&TorusShader, "Torus");
			figuresVector.AddFigure(torus);
		}
	}
	ImGui::End();
}

void Manager::Draw()
{
	for (int i = 0; i < figuresVector.figures.size(); i++)
	{
		if (figuresVector.figures[i]->shader != currentShader)
		{
			currentShader = figuresVector.figures[i]->shader;
			currentShader->Activate();
			currentCamera->SaveMatrixToShader(*currentShader);
		}

		if(currentCamera->HasBeenUpdated())
			currentCamera->SaveMatrixToShader(*currentShader);

		bool show = i == selectedTorus;
		if (show)
			figuresVector.figures[i]->Mark();
		figuresVector.figures[i]->Draw(show);
		figuresVector.figures[i]->UnMark();
	}
}

#pragma once
#include "Figure.h"
#include "Camera.h"
#include "Torus.h"
#include "Libraries/include/ImGui/imgui.h"


class Manager {
public:
	class FiguresVector {
	public:
		std::vector<Figure*> figures;
		std::vector< const char*> names;

		void AddFigure(Figure* figure) {
			figures.push_back(figure);
			names.push_back(figure->name);
		}

		void Delete() {
			for (int i = 0; i < figures.size(); i++)
				delete figures[i];
		}
	};
	FiguresVector figuresVector;
	Camera* currentCamera;
	Shader* currentShader;
	Shader TorusShader;

	Manager(Camera* camera);
	~Manager();
	void MenuInterferes();
	void Draw();
private:
	int selectedTorus = -1;
};
#pragma once
#include "Figure.h"
#include "Camera.h"
#include "Torus.h"
#include "Point.h"
#include "Cursor.h"
#include "Libraries/include/ImGui/imgui.h"
#include "Shader.h"
#include "OpenGLHelper.h"
#include "CanterPoint.h"
#include "BazierCurve.h"
//#include <GLFW/glcorearb.h>

class Manager {
	class FiguresVector {
	public:
		std::vector<Figure*> figures;

		std::vector< const char*> names;
		std::vector<bool> active;

		int NumberOfActive() {
			return activeCount;
		}

		void AddFigure(Figure* figure) {
			figures.push_back(figure);
			names.push_back(figure->name);
			active.push_back(false);
		}

		void DeleteFigure(int id) {
			delete figures[id];
			figures.erase(std::next(figures.begin(), id));
			names.erase(std::next(names.begin(), id));

			if (active[id])
				ChangeActiveState(id);
			active.erase(std::next(active.begin(), id));
		}

		bool ChangeActiveState(int i) {
			active[i] = !active[i];

			if (active[i])
				activeCount++;
			else
				activeCount--;
			return active[i];
		}

		int Size() {
			return figures.size();
		}

		~FiguresVector() {
			Delete();
		}

		void Delete() {
			for (int i = 0; i < figures.size(); i++)
				delete figures[i];
		}
	private:
		int activeCount;
	};

	FiguresVector figuresVector;
	Camera* currentCamera;
	Shader* currentShader;
	Shader TorusShader; // fajnie jakbym nie musia³ definiowaæ za ka¿dym razem tych shaderów, mo¿e statyczny shader w clasie
	Shader PointShader;
	Shader CursorShader;
	Shader BezierShader;

	CenterPoint centerPoint;

	Cursor cursor;
	GLFWwindow* window;
	bool mouseLeftFirstClick = true;

	float minLengthFromMouse = 0.0004f;
	int TheClosetFigureToMouse(const char* figureType);
	
	void CreateFiguresInterfers();
	void SelectableList();
	void DeleteSelected();

	void Select(int i);
	//void DrawSpecialFigure(Figure& figure);

public:
	Manager(Camera* camera, GLFWwindow* window);
	void MenuInterferes();
	void Draw();
	void ProcesInput();
	

	~Manager();
};
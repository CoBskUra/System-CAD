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
		std::map<std::string ,FigureContainer*> activeFigureContainers;
		std::map<std::string, FigureContainer*> figureContainers;

		std::vector< const char*> names;
		std::vector<bool> active;

		FigureContainer* testContainer;

		int NumberOfActive() {
			return activeCount;
		}

		void AddFigure(Figure* figure) {
			figures.push_back(figure);
			names.push_back(figure->name);

			if (figure->GetType() == "BezierCurve")
			{
				figureContainers.insert({ figure->GetUniqueName(), (BezierCurve*)figure });
			}
			else {
				std::map<std::string, FigureContainer*>::iterator iter;
				for (iter = figureContainers.begin(); iter != figureContainers.end(); iter++) {
					iter->second->Add(figure);
				}
			}
			active.push_back(false);
		}

		void DeleteFigure(int id) {
			if (active[id])
				ChangeActiveState(id);
			active.erase(std::next(active.begin(), id));

			auto toDelete = figures[id];
			figures.erase(std::next(figures.begin(), id));
			names.erase(std::next(names.begin(), id));
			delete toDelete;
		}

		bool ChangeActiveState(int i) {
			active[i] = !active[i];

			if (active[i]) {
				activeCount++;
				auto a = figureContainers.find(figures.at(i)->GetUniqueName());
				if (a != figureContainers.end())
					activeFigureContainers.insert({ a->first, a->second });
			}
			else {
				activeCount--;
				activeFigureContainers.erase(figures.at(i)->GetUniqueName());
			}
			return active[i];
		}

		int Size() {
			return figures.size();
		}

		~FiguresVector() {
			Delete();
		}

		void Delete() {
			figureContainers.erase(figureContainers.begin(), figureContainers.end());
			activeFigureContainers.erase(activeFigureContainers.begin(), activeFigureContainers.end());

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
	glm::vec2 mouseLastPosition;

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
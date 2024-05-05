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
#include "BezierC0.h"
#include "BezierC2.h"
#include "FiguresVector.h"
//#include <GLFW/glcorearb.h>

class Manager {
	FiguresVector figuresVector;
	Camera* currentCamera;

	CenterPoint centerPoint;

	Cursor cursor;
	GLFWwindow* window;
	bool mouseLeftFirstClick = true;
	glm::vec2 mouseLastPosition;

	float minLengthFromMouse = 0.001f;
	int TheClosetFigureToMouse(FigureType figureType);
	
	void CreateFiguresInterfers();
	void SelectableList();

	void Select(int i);
public:
	Manager(Camera* camera, GLFWwindow* window);
	void MenuInterferes();
	void Draw();
	void ProcesInput();

	~Manager();
};